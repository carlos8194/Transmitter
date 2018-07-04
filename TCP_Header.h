//
// Created by carlos on 24/06/18.
//

#ifndef TRANSMITTER_TCP_HEADER_H
#define TRANSMITTER_TCP_HEADER_H


class TCP_Header {


private:
    unsigned sequence;
    unsigned ack;
    short window;
};


#endif //TRANSMITTER_TCP_HEADER_H
