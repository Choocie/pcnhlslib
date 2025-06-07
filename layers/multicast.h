#include "hls_stream.h"

template<typename T,int PAR>
void multicast(hls::stream<T> &in, hls::stream<T> out[PAR]) {
    #pragma HLS inline off
    #pragma HLS pipeline II=1
    T data;
    in >> data;
    for(int p = 0; p < PAR; p++) {
        #pragma HLS unroll
        out[p] << data;
    }
}

template<typename T,int PAR, int DOUT>
void multicast(hls::stream<T> &in, hls::stream<T,DOUT> out[PAR]) {
    #pragma HLS inline off
    #pragma HLS pipeline II=1
    T data;
    in >> data;
    for(int p = 0; p < PAR; p++) {
        #pragma HLS unroll
        out[p] << data;
    }
}

template<typename T,int PAR, int II>
void multicast(hls::stream<T> &in, hls::stream<T> out[PAR]) {
    for(int ii =0; ii < II; ii++){
        #pragma HLS pipeline II=1 rewind
        T data;
        in >> data;
        for(int p = 0; p < PAR; p++) {
            out[p] << data;
        }
    }
}


template<typename T,int PAR, int II, int DOUT>
void multicast(hls::stream<T> &in, hls::stream<T,DOUT> out[PAR]) {
    for(int ii =0; ii < II; ii++){
        #pragma HLS pipeline II=1 rewind
        T data;
        in >> data;
        for(int p = 0; p < PAR; p++) {
            out[p] << data;
        }
    }
}

template<typename T,int IN_PAR, int OUT_PAR, int II>
void multicast(hls::stream<T> in[IN_PAR], hls::stream<T> out[OUT_PAR][IN_PAR]) {
    for(int ii =0; ii < II; ii++){
        #pragma HLS pipeline II=1 rewind
        for(int i = 0; i < IN_PAR;i++) {
            T data;
            in[i] >> data;
            for(int p = 0; p < OUT_PAR; p++) {
                out[p][i] << data;
            }
        }
    }
}

template<typename T,int IN_PAR, int OUT_PAR, int II, int DOUT>
void multicast(hls::stream<T> in[IN_PAR], hls::stream<T,DOUT> out[OUT_PAR][IN_PAR]) {
    for(int ii =0; ii < II; ii++){
        #pragma HLS pipeline II=1 rewind
        for(int i = 0; i < IN_PAR;i++) {
            T data;
            in[i] >> data;
            for(int p = 0; p < OUT_PAR; p++) {
                out[p][i] << data;
            }
        }
    }
}
