#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
/*enum  {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};*/

static int SwitchPin = 17;

//static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_task(void);

/*------------- MAIN -------------*/
int main(void)
{
    stdio_init_all();

    printf("DEBUG: starting up HID device...\n");

    board_init();
    tusb_init();
    gpio_init(SwitchPin);
    gpio_set_dir(SwitchPin,GPIO_IN);
    gpio_pull_down(SwitchPin);
    printf("DEBUG: HID Device initialized\n");

    while (1)
    {
        tud_task(); // tinyusb device task
        //led_blinking_task();

        hid_task();
    }

    return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
    //blink_interval_ms = BLINK_MOUNTED;
    printf("DEBUG: Mounted\n");
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    //blink_interval_ms = BLINK_NOT_MOUNTED;
    printf("DEBUG: Unmounted\n");
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    //blink_interval_ms = BLINK_SUSPENDED;
    printf("DEBUG: Suspended\n");
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    //blink_interval_ms = BLINK_MOUNTED;
    printf("DEBUG: Resumed Mounted\n"); 
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if ( board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = board_button_read();

    // Remote wakeup
    if ( tud_suspended() && btn )
    {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }else
    {
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        if ( !tud_hid_ready() ) return;

        hid_gamepad_report_t report =
        {
            .x = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0, .hat = 0, .buttons = 0
        };
        if (gpio_get(SwitchPin)){
            report.buttons = GAMEPAD_BUTTON_A;
            report.hat = GAMEPAD_HAT_UP;
            tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
        } else {
            report.buttons = 0;
            report.hat = GAMEPAD_HAT_CENTERED;
            tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
        }
         
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t itf, uint8_t const* report, uint8_t len)
{
    (void) itf;
    (void) len;

    printf("DEBUG: REPORT Sent Successfully to Host\n");
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) itf;
        
    printf("DEBUG: tud_hid_set_report_cb triggered\n");
    printf("DEBUG: report_id: %X\n", report_id);
    printf("DEBUG: report_type: %X\n", report_type);
    printf("DEBUG: bufsize: %d\n", bufsize);

    printf("DEBUG: buffer content:\n");
    for (int i = 0; i < bufsize; i++) {
        printf("%02X ", buffer[i]);
    }
    printf(" - End \n");
    
    /*if (report_type == HID_REPORT_TYPE_OUTPUT)
    {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD)
        {
            // bufsize should be (at least) 1
            if ( bufsize < 1 ) return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
            {
                // Capslock On: disable blink, turn led on
                blink_interval_ms = 0;
                board_led_write(true);
            } else
            {
                // Caplocks Off: back to normal blink
                board_led_write(false);
                blink_interval_ms = BLINK_MOUNTED;
            }
        }
    }*/
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
/*void led_blinking_task(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // blink is disabled
    if (!blink_interval_ms) return;

    // Blink every interval ms
    if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}*/