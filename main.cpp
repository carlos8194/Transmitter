#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include "TCP_Header.h"

#define BETA 2
#define ALFA 0.5
#define PACKET_SIZE 1
#define DEFAULT_PORT 9090
#define LOSS_PACKET_PROB 0.2
#define PACKET_SEND_TIME 1
#define INIT_SEQUENCE 1

using namespace std;

double RTT = 7;
clock_t t1;
clock_t t2;
mutex wait_lock;
mutex window_lock;
condition_variable condition_var;
unsigned short window = 10;
unsigned currentSequence = INIT_SEQUENCE;
unsigned time_out = static_cast<unsigned>(RTT * BETA);
list<unsigned> window_list;
unsigned window_start_seq = INIT_SEQUENCE;
bool ack_received = false;
bool retransmitting = false;

int sock;
ssize_t n;
unsigned length;
struct sockaddr_in server, from;
struct hostent *hp;

void error(const char*);
void ack_listener();

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: server port\n");
        exit(1);
    }
    sock= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        error("socket");
    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp == nullptr)
        error("Unknown host");

    bcopy(hp->h_addr, (char *)&server.sin_addr, static_cast<size_t>(hp->h_length));
    server.sin_port = htons(DEFAULT_PORT);
    length = sizeof(struct sockaddr_in);

    // Random number seed
    srand (static_cast<unsigned int>(time(nullptr)));
    int randNumber;

    // Lock to wait for a condition
    unique_lock<mutex> my_lock(wait_lock);
    thread listener(ack_listener);

    while (true) {
        // Send the whole window, then wait for the ACK or until time out
        window_lock.lock();
        for (unsigned short i = 0; i < window; ++i) {
            // Send a packet at a time.
            this_thread::sleep_for(chrono::seconds(PACKET_SEND_TIME)); // Wait 1 second
            randNumber = rand() % 10;

            // Depending on the retransmitting condition adjust the sequence numbers.
            if(retransmitting){
                currentSequence = window_start_seq;
                cout << "Retransmitting window. First Seq: " << currentSequence << endl;
            } else{
                window_start_seq = currentSequence;
            }

             //Sends a packet. It may be lost on its way.
            if (randNumber < (1 - LOSS_PACKET_PROB)*10) {
                TCP_Header tcp_header(currentSequence, 0, window, false);
                auto packet = tcp_header.header_to_Array();
                n = sendto(sock, packet, HEADER_SIZE, 0, (const struct sockaddr *) &server, length);
                if (n < 0)
                    error("Sendto");
                delete[] packet;
                cout << "Send packet with Seq: " << currentSequence << endl;
            }

            if (!retransmitting){
                window_list.push_back(currentSequence);
            }
            currentSequence += PACKET_SIZE;
        }
        cout << "Sent the window up to ACK seq: " << (currentSequence - 1) << endl;
        t1 = time(nullptr);
        window_lock.unlock();

        condition_var.wait_for(my_lock, chrono::seconds(time_out));

        window_lock.lock();
        // Check if time ran out to start retransmitting.
        retransmitting = !ack_received;
        ack_received = false;
        window_lock.unlock();
    }
    // close(sock);
}

void ack_listener() {
    while (true) {
        char message[HEADER_SIZE];
        n = recvfrom(sock, message, HEADER_SIZE, 0, (struct sockaddr *) &from, &length);
        if (n < 0)
            error("recvfrom");
        window_lock.lock();
        ack_received = true;
        condition_var.notify_one();
        TCP_Header header(message);
        window = header.getWindow();
        currentSequence = header.getAck();
        cout << "Received an ACK. Window: " << window << " ACK Num: " << currentSequence << endl;
        if (!retransmitting){
            t2 = time(nullptr);
            double RTTnew = (t2 - t1)/1000;
            RTT = (1 - ALFA)*RTTnew + ALFA*RTT;
            time_out = static_cast<unsigned>(RTT * BETA);
            cout << "New RTT: " << RTTnew << " Updated to " << RTT << endl;
            cout << "New timeOut: " << time_out << endl;
        }
        window_list.clear();
        retransmitting = false;
        window_lock.unlock();
    }
}