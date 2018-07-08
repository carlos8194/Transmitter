//
// Created by carlos on 24/06/18.
//

#include "TCP_Header.h"

TCP_Header::TCP_Header(const char* str) {
    const char* array = str;
    this->sequence = charArrayToUnsigned(array);
    array += 4;
    this->ack = charArrayToUnsigned(array);
    array += 4;
    this->window = charArrayToShort(array);
    array += 2;
    isACK = (array[0] == 1);
}

TCP_Header::TCP_Header(unsigned seq, unsigned ACK, unsigned short wd, bool isACK) {
    this->ack = ACK;
    this->isACK = isACK;
    this->sequence = seq;
    this->window = wd;
}

unsigned int TCP_Header::getSequence() const {
    return sequence;
}

unsigned int TCP_Header::getAck() const {
    return ack;
}

unsigned short TCP_Header::getWindow() const {
    return window;
}

bool TCP_Header::IsACK() const {
    return isACK;
}

char *TCP_Header::header_to_Array() {
    auto *array = new char[HEADER_SIZE];
    char *aux_array = array;
    writeUnsignedToArray(sequence, aux_array);
    aux_array += 4;
    writeUnsignedToArray(ack, aux_array);
    aux_array += 4;
    writeShortToArray(window, aux_array);
    aux_array += 2;
    auto ack_flag = (char) ((isACK) ? 1 : 0);
    aux_array[0] = ack_flag;
    return array;
}

unsigned short TCP_Header::charArrayToShort(const char *str) {
    unsigned short val = 0;
    val += ( unsigned char) str[0];
    val += ((unsigned char) str[1]) << 8; // *256
    return val;
}

unsigned TCP_Header::charArrayToUnsigned(const char *str) {
    unsigned val = 0;
    val += ( unsigned char) str[0];
    val += ((unsigned char) str[1]) << 8;  // *256
    val += ((unsigned char) str[2]) << 16; // *256^2
    val += ((unsigned char) str[3]) << 24; // *256^3
    return val;
}

void TCP_Header::writeShortToArray(unsigned short num, char *str) {
    unsigned short oNum = num >> 8;
    auto b0 = (char) (num - (oNum << 8));
    auto b1 = (char) oNum;
    str[0] = b0;
    str[1] = b1;
}

void TCP_Header::writeUnsignedToArray(unsigned num, char *str) {
    for (int i = 0; i < 4; ++i) {
        unsigned oNum = num >> 8;
        auto byte = (char) (num - (oNum << 8));
        str[i] = byte;
        num = oNum;
    }
}