//
// Created by carlos on 24/06/18.
//

#ifndef RECEIVER_TCP_HEADER_H
#define RECEIVER_TCP_HEADER_H

#define HEADER_SIZE     128 // Maximum value for the header

class TCP_Header {

public:
    explicit TCP_Header(const char*);
    TCP_Header(unsigned seq, unsigned ACK, unsigned short wd, bool isACK);
    unsigned int getSequence() const;
    unsigned int getAck() const;
    unsigned short getWindow() const;
    bool IsACK() const;
    char* header_to_Array();

private:
    unsigned short charArrayToShort(const char*);
    unsigned charArrayToUnsigned(const char*);
    void writeShortToArray(unsigned short, char*);
    void writeUnsignedToArray(unsigned, char*);

    unsigned sequence;
    unsigned ack;
    unsigned short window;
    bool isACK;
};

#endif //RECEIVER_TCP_HEADER_H