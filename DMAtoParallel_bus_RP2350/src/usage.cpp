///////////////////////////////////////////////////////////////////////////////////
// This programme is an 800x480 LCD driver (parallel bus) controlled by NT35510.
// Do not define the following #define on boards without PSRAM.
///////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include "nt35510.hpp"
NT35510LCD lcd;

void setup_debug_serial_out(void) {
    #define DBG Serial1.printf
    Serial1.setRX(17u);
    Serial1.setTX(16u);
    Serial1.begin(9600);
}

inline uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3);
}

void setup()
{
    //sem_init(&bitblt_sem, 1, 1);

    volatile int psram_size;
    setup_debug_serial_out();
    
    psram_size = rp2040.getPSRAMSize();
    DBG("PSRAM size:%d\n", psram_size);

    // load png to draw on chip map (PSRAM)
    lcd.pngle = pngle_new();
    pngle_set_draw_callback(lcd.pngle, lcd.pngle_on_draw);
    lcd.loadpng(pngdata);

    lcd.setup_gpio();
    // reset DEBUG_SIG
    lcd.dma_irq_handler();

    // NT35510初期化
    lcd.init_LCD();    
    
    // PIOとDMAの初期化
    PIO pio = pio0;
    uint sm = 0;
    for(int i = 0; i < 7; i++) {
        dma_channel[i] = dma_claim_unused_channel(true);
    }
    uint offset = pio_add_program(pio, &parallel_out_program);

    lcd.setup_pio(pio, sm, offset);
    lcd.setup_dma(pio, sm);

    // 最初のDMAチャンネルを起動
    lcd.initframebuffers();
    sleep_us(1000);
    dma_channel_wait_for_finish_blocking(dma_channel[1]);
    dma_channel_start(dma_channel[0]);
    
}

const uint16_t map_table[][2] = {
    //  Y         X
    { 7 * 24,  0 * 24},
    { 7 * 24,  1 * 24},
    { 7 * 24,  2 * 24},
    { 7 * 24,  3 * 24},
    { 7 * 24,  4 * 24},
    { 7 * 24,  5 * 24},
    { 7 * 24,  6 * 24},
    { 7 * 24,  7 * 24},
    { 7 * 24,  8 * 24},
    { 7 * 24,  9 * 24},
};

const uint8_t field_map[32][16] = {
    {0x01, 0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x01}, 
    {0x05, 0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x01, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x01, 0x01, 0x01, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x01, 0x00, 0x01, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 

    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x01, 0x01, 0x01, 0x05}, 
    {0x05, 0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x09, 0x05, 0x05, 0x05, 0x05}, 
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 

};

bool loop_started = false;
void loop() {
    loop_started = true;
    static int a;

    if(++a == 2) {
        a = 0;
    }

    if(a) {
        sio_hw->gpio_set = (1ul << 25) | 1;
    } else {
        sio_hw->gpio_clr = (1ul << 25) | 1;
    }

// drawing test
    static int cnt = 15;
#if 0
    for(int i = 0; i < 8; i++) {
        lcd.line(384 + random(416), random(480), 384 + random(416), random(480), rgb(random(128), random(128), random(128)), true);
        lcd.box(384 + random(416), random(480), 384 + random(416), random(480), rgb(random(128), random(128), random(128)), true);
        lcd.circle(384 + random(416), random(480), random(480), rgb(random(128), random(128), random(128)), true);
    }
#endif

    static bool first_draw_done = false;
    for(int y = 0; y < 16; y++) {
        for(int x = 0; x < 16; x++) {
            uint16_t old_chip_map_y = abs(y + cnt + 1) % 16;
            uint16_t old_table_x = map_table[field_map[old_chip_map_y][x]][1];
            uint16_t old_table_y = map_table[field_map[old_chip_map_y][x]][0];
            uint16_t chip_map_y = abs(y + cnt) % 16;
            uint16_t table_x = map_table[field_map[chip_map_y][x]][1];
            uint16_t table_y = map_table[field_map[chip_map_y][x]][0];
            // これから書くチップがすでに書き込み先チップと違う時だけ実際に書く。
            if(!first_draw_done || (old_table_y != table_y || old_table_x != table_x)) {
                lcd.bitblt(chip_map, x * 24, y * 24, table_x, table_y, 24, 24, true);
            }
        }
    }
    first_draw_done = true;
    cnt--;if(cnt < 0){cnt = 15;}
    // debug
    digitalWrite(TFT_DEBUG_SIG, HIGH);
    digitalWrite(TFT_DEBUG_SIG, LOW);
    digitalWrite(TFT_DEBUG_SIG, HIGH);
    digitalWrite(TFT_DEBUG_SIG, LOW);

    lcd.swapbuffer();
    
    //sleep_us(10); // 適当に遅延
}

void setup1(void) {
}

void loop1(void) {
    if(!loop_started) {
        return;
    }
// drawing test
    static int cnt = 15;
#if 0
    for(int i = 0; i < 8; i++) {
        lcd.line(384 + random(416), random(480), 384 + random(416), random(480), rgb(random(128), random(128), random(128)), true);
        lcd.box(384 + random(416), random(480), 384 + random(416), random(480), rgb(random(128), random(128), random(128)), true);
        lcd.circle(384 + random(416), random(480), random(480), rgb(random(128), random(128), random(128)), true);
    }
#endif

    static bool first_draw_done = false;
    for(int y = 0; y < 16; y++) {
        for(int x = 0; x < 16; x++) {
            uint16_t old_chip_map_y = abs(y + cnt + 1) % 16;
            uint16_t old_table_x = map_table[field_map[old_chip_map_y][x]][1];
            uint16_t old_table_y = map_table[field_map[old_chip_map_y][x]][0];
            uint16_t chip_map_y = abs(y + cnt) % 16;
            uint16_t table_x = map_table[field_map[chip_map_y][x]][1];
            uint16_t table_y = map_table[field_map[chip_map_y][x]][0];
            // Only actually draw if the chip about to be drawn is different than the chip
            // already in the drawing destination.
            if(!first_draw_done || (old_table_y != table_y || old_table_x != table_x)) {
                lcd.bitblt(chip_map, 400 + x * 24, y * 24, table_x, table_y, 24, 24, true);
                //lcd.sprite(400 + x * 24, y * 24, table_x, table_y, 24, 24, 0, true);
            }
        }
    }
    first_draw_done = true;
    cnt--;if(cnt < 0){cnt = 15;}

}
