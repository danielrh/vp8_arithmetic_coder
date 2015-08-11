#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitreader.h"
#include "bitwriter.h"

uint8_t bitvector[131072 * 8 - 1024] = {0};
uint8_t obitvector[sizeof(bitvector)];
uint8_t wbuffer[131072 * 2] = {0};
void populate_test_buffer() {
    FILE * fp = fopen("/dev/urandom", "r");
    fread(bitvector, 1, sizeof(bitvector), fp);
    fclose(fp);
    for (size_t i = 0; i < sizeof(bitvector); ++i) {
        bitvector[i] &= 1;
    }
}
void test_arithmetic_codingA() {
    (void)vpx_read_literal;
    (void)vpx_write_literal;
    for (size_t simple_size = 0; simple_size < 128; ++ simple_size) {
        for (size_t i = 0; i < simple_size; ++i) {
            bitvector[i] = i;
            bitvector[i] &= 1;
        }
        vpx_writer wri ={0};
        vpx_start_encode(&wri, wbuffer);
        for (size_t i = 0; i < simple_size; ++i) {
            vpx_write(&wri, bitvector[i], 128);//i & 255);
        }
        vpx_stop_encode(&wri);
        vpx_reader rea={0};
        vpx_reader_init(&rea,
                        wri.buffer,
                        wri.pos);
        for (size_t i = 0; i < simple_size; ++i) { // short enough buffer that we don't get overlap
            obitvector[i] = vpx_read(&rea, 128);//i & 255);
            if (obitvector[i] != bitvector[i]) {
                fprintf(stderr, "FAILURE[%d] %x != %x\n", (int)i, bitvector[i], obitvector[i]);
                assert(false);
            }
        }
        assert(vpx_reader_has_error(&rea) == 0);
        assert(memcmp(obitvector, bitvector, simple_size) == 0);
    }
}

void test_arithmetic_coding() {
    populate_test_buffer();
    vpx_writer wri ={0};
    vpx_start_encode(&wri, wbuffer);
    for (size_t i = 0; i < sizeof(bitvector); ++i) {
        vpx_write(&wri, bitvector[i], i & 255);
    }
    vpx_stop_encode(&wri);
    vpx_reader rea={0};
    vpx_reader_init(&rea,
                    wri.buffer,
                    wri.pos);
    for (size_t i = 0; i < sizeof(bitvector); ++i) {
        obitvector[i] = vpx_read(&rea, i & 255);
        if (obitvector[i] != bitvector[i]) {
            fprintf(stderr, "FAILURE[%d] %x != %x\n", (int)i, bitvector[i], obitvector[i]);
            assert(false);
        }
    }
    assert(vpx_reader_has_error(&rea) == 0);
    assert(memcmp(obitvector, bitvector, sizeof(bitvector)) == 0);
}

int main () {
    test_arithmetic_codingA();
    test_arithmetic_coding();
    return 0;
}
