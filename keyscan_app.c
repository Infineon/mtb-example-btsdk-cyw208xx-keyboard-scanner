/******************************************************************************
* File Name:  keyscan_app.c
*
* Description:  This code example demonstrates the dedicated keyscan hardware
* which processes key press events from LHL GPIOs without
* interrupting the CPU core. This code example also demonstrates
* low power entry when the system is idle (no activity/keypress)
* and wakes up when there is key press.
*
* Related Document: See Readme.md
*
********************************************************************************
* Copyright 2020-2022, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
 *  @file
 *  keyscan_app.c
 *
 * Features demonstrated
 *  - Functionality of Keyscan and low power application.
 *
 * To demonstrate the app, work through the following steps.
 * 1. Plug the CYW9208xxEVB-02 evaluation board to your computer.
 * 2. Build and download the application. (See CYW9208xxEVB-02 User Guide)
 * 3. Connect a matrix button keys to the corresponding rows(P0-P7) and
 *    the supermuxed columns(any of P0-P39) on the evaluation board. (or)
 *    In case, there is no availability of matrix button keys, bring the
 *    corresponding row and column pins in contact by flywiring.
 * 4. Use Terminal emulation tools like Teraterm or Putty to view the
 *    trace messages(Refer Kit User Guide).
 *
 ******************************************************************************/

/*******************************************************************************
 *                      Includes
 ******************************************************************************/

#include "wiced_bt_dev.h"
#include "wiced_bt_stack.h"
#include "wiced_bt_trace.h"
#include "wiced_hal_gpio.h"
#include "wiced_hal_keyscan.h"
#include "wiced_hal_mia.h"
#include "wiced_timer.h"
#include "wiced_hal_puart.h"
#include "wiced_sleep.h"
#include "wiced.h"

/*******************************************************************************
 *                      Macros
 ******************************************************************************/
/* Macro to enable and disable low power */
#define LOW_POWER_ENABLE

/*
 * Number of Px pins to be configured as rows.
 * Pins are configured from P0 to P< NUM_KEYSCAN_ROWS - 1 >
 * Maximum allowable number is 8 i.e till P7.
 * These pins are not super-Muxable.
 * Please refer to the datasheet for more details.
 */
#define NUM_KEYSCAN_ROWS                        7

/*
 * Number of Px pins to be configured as columns.
 * Pins are configured from P8 to P< 8 + NUM_KEYSCAN_COLS - 1 > by default.
 * Maximum allowable number is 20. These pins are super-Muxable.
 * Using wiced_hal_gpio_select_function(), select the suitable
 * LHL GPIOs and KSO#s. Please refer to the datasheet for more details.
 */
#define NUM_KEYSCAN_COLS                        9

/*
 * Timeout in milliseconds for keydown_repeat_timer -
 * To monitor the key press time of a keyscan button on continuous press.
 */
#define KEYDOWN_EVENT_REPEAT_MONITOR_MS         100

/*
 * To check if there are any active key presses and events pending
 */
#define keyscanActive() (wiced_hal_keyscan_is_any_key_pressed() || \
                         wiced_hal_keyscan_events_pending())


/*******************************************************************************
 *               Variable Declarations
 ******************************************************************************/

/* milliseconds periodic timer instance */
static wiced_timer_t            keydown_repeat_timer;

/* Keycode refers to the integer value that is invoked on a keypress
 * Keycode value increases by row-major order
 */
static uint8_t                  volatile keycode = EVENT_NONE;

/* counter increments on every KEYDOWN_EVENT_REPEAT_MONITOR_MS for keypress */
static uint8_t                  counter = 0;

#ifdef LOW_POWER_ENABLE
/*
 * app_sleep_config is used for configuring the device sleep and wake-up
 * parameters
 */
static wiced_sleep_config_t     app_sleep_config;
#endif

/*******************************************************************************
 *               Function Declarations
 ******************************************************************************/
static void kbapp_map_columns(void);

static void kbapp_procErrKeyscan(void);

static void kbapp_keyDownRepeatMonitor_cb(uint32_t unused);

static void kbapp_processKeyEvents(void* unused);

static void kbapp_init(void);

static wiced_result_t
kb_management_cback(wiced_bt_management_evt_t event,
                    wiced_bt_management_evt_data_t *p_event_data);

#ifdef LOW_POWER_ENABLE
static void post_sleep_callback (wiced_bool_t restore_configuration);
static wiced_result_t
keyscan_configure_sleep(wiced_sleep_config_t app_sleep_config);

static uint32_t keyscan_sleep_handler(wiced_sleep_poll_type_t type);
#endif

/*******************************************************************************
 *               Function Definitions
 ******************************************************************************/

/*******************************************************************************
 * Function Name: kbapp_map_columns
 * @brief kbapp_map_columns() functions maps the Keyscan columns to the desired
 *        LHL GPIOs and this is required when the device wakes up from sleep.
 *
 * @return void
*******************************************************************************/
static void kbapp_map_columns(void)
{

    wiced_hal_gpio_select_function(WICED_P08, WICED_KSO0);
    wiced_hal_gpio_select_function(WICED_P09, WICED_KSO1);
    wiced_hal_gpio_select_function(WICED_P10, WICED_KSO2);
    wiced_hal_gpio_select_function(WICED_P11, WICED_KSO3);
    wiced_hal_gpio_select_function(WICED_P12, WICED_KSO4);
    wiced_hal_gpio_select_function(WICED_P13, WICED_KSO5);
    wiced_hal_gpio_select_function(WICED_P14, WICED_KSO6);
    wiced_hal_gpio_select_function(WICED_P15, WICED_KSO7);
    wiced_hal_gpio_select_function(WICED_P17, WICED_KSO8);

}

/*******************************************************************************
 * Function Name: kbapp_procErrKeyscan

 Function Description:
 @brief  This function handles error events reported by the keyscan HW.
         Typically these would be ghost events.
         In addition, it also performs the following actions:
            - All pending events are flushed
            - The keyscan HW is reset
        This function is typically used when the FW itself is (or involved in)
        in error. In such cases the FW no longer has the correct state of
        anything and we must resort to a total reset. This function is invoked
        from "kbapp_processKeyEvents" function.

 @param  unused  parameter that can be used in this callback function if needed.

 @return void
 ******************************************************************************/
static void kbapp_procErrKeyscan(void)
{

    WICED_BT_TRACE("KeyScan Error detected\r\n");

     /* Flush the event fifo */
    wiced_hal_keyscan_flush_HW_events();

     /* Reset the keyscan HW */
    wiced_hal_keyscan_reset();

     /*Configure GPIOs for keyscan operation*/
    wiced_hal_keyscan_config_gpios();

}


/*******************************************************************************
 * Function Name: kbapp_keyDownRepeatMonitor_cb

 Function Description:
 @brief  Keydown repeat event monitor function. This callback function will be
         initiated by "kb_management_cback" every "keyboard_repeat_timer"
         milliseconds.

 @param  unused  parameter that can be utilized by this callback function if needed.

 @return void
 ******************************************************************************/
static void kbapp_keyDownRepeatMonitor_cb(uint32_t unused)
{
    /*
     * counter variable  increments for every 100 milliseconds when a key is
     * pressed continously
     */
    WICED_BT_TRACE("\r\nKey : %d Row#: %d, Col#: %d "
                   "pressed for %d milliseconds \r\n",
                    keycode,
                    keycode % NUM_KEYSCAN_ROWS,
                    keycode / NUM_KEYSCAN_ROWS,
                    ((++counter) * KEYDOWN_EVENT_REPEAT_MONITOR_MS));
}


/*******************************************************************************
 * Function Name: kbapp_processKeyEvents

 Function Description:
 @brief  This function polls for key activity and queues any key events in the
         FW event queue.Events from the keyscan driver are processed until the
         driver runs out of events.

 @param  unused  parameter that can be utilized by this callback function
                 when needed

 @return void
 ******************************************************************************/
static void kbapp_processKeyEvents(void* unused)
{

    KeyEvent keyEvent = { 0, 0, 0, 0 };

     /* Poll the hardware for events */
    wiced_hal_mia_pollHardware();

     /* Process all key events from the keyscan driver */
    while(wiced_hal_keyscan_get_next_event(&keyEvent))
    {
        /* Checks whether key is pressed or released */
        if(keyEvent.upDownFlag)
        {
             /* Display the keycode when key is released */
            if((EVENT_NONE != keyEvent.keyCode) &&
               (END_OF_SCAN_CYCLE != keyEvent.keyCode) &&
               (ROLLOVER != keyEvent.keyCode))
            {
                WICED_BT_TRACE("Key = %d: Released \r\n", keyEvent.keyCode);
                /* Check whether any key is pressed */
                if (wiced_hal_keyscan_is_any_key_pressed())
                {
                    /* Reset count when key is released */
                    counter = 0;
                }
                /* Stop timer if key is released */
                wiced_stop_timer(&keydown_repeat_timer);
            }

        }
        else
        {
            if((EVENT_NONE != keyEvent.keyCode) &&
               (END_OF_SCAN_CYCLE != keyEvent.keyCode))
            {
                keycode = keyEvent.keyCode;

                /* Check if keycode is valid */
                if((NUM_KEYSCAN_ROWS * NUM_KEYSCAN_COLS) <= keycode)
                {
                    WICED_BT_TRACE("Keycode : %d ", keycode);
                    kbapp_procErrKeyscan();
                    return;
                }
                /* Reset count when key is pressed */
                counter = 0;

                /*
                 * Start Timer to indicate repeated keydown events and
                 * configure milliseconds periodic timer and start timer with
                 * keydown_repeat_timer
                 */
                if(WICED_SUCCESS != wiced_start_timer(&keydown_repeat_timer,
                                               KEYDOWN_EVENT_REPEAT_MONITOR_MS))
                {
                    WICED_BT_TRACE("\r\n keydown_repeat_timer timer "
                                   "start failure\r\n");
                    return;
                }
            }
        } /* if key up/down */
    } /* while loop to process up/down keyevents from the keyscan block */
} /* kbapp_processKeyEvents */


/*******************************************************************************
 * Function Name: kbapp_init
 * @brief kbapp_init function is the entry point for the keyboard scanner
 *        application logic.
 *
 * @param void
 *
 * @return void
*******************************************************************************/
static void kbapp_init(void)
{

        /* Configure number of rows and columns for keyscan */
        wiced_hal_keyscan_configure(NUM_KEYSCAN_ROWS, NUM_KEYSCAN_COLS);

        /* Initialize keyscan */
        wiced_hal_keyscan_init();

        /* Usage Details*/
        WICED_BT_TRACE( "\r\n------------------------\r\n"
                        "ROW#        Mapped Px#\r\n"
                        "------------------------\r\n"
                        "0           P0\r\n"
                        "1           P1\r\n"
                        "2           P2\r\n"
                        "3           P3\r\n"
                        "4           P4\r\n"
                        "5           P5\r\n"
                        "6           P6\r\n"
                        "------------------------\r\n"
                        "COL#        Mapped Px#\r\n"
                        "------------------------\r\n"
                        "0           P8\r\n"
                        "1           P9\r\n"
                        "2           P10\r\n"
                        "3           P11\r\n"
                        "4           P12\r\n"
                        "5           P13\r\n"
                        "6           P14\r\n"
                        "7           P15\r\n"
                        "8           P17\r\n\r\n" );

        /* User Prompt for Initial Key press */
        WICED_BT_TRACE("\r\nPress any key by shorting one row pin and "
                       "column pin from the above table..\r\n");

        /* Initialize timer to track key pressed time */
        if(WICED_SUCCESS != wiced_init_timer(&keydown_repeat_timer,
                                            &kbapp_keyDownRepeatMonitor_cb,
                                            0,
                                            WICED_MILLI_SECONDS_PERIODIC_TIMER))
        {
            WICED_BT_TRACE("\r\n keydown_repeat_timer init failed\r\n");
        }

        /* Register for event notification callback */
        wiced_hal_keyscan_register_for_event_notification(kbapp_processKeyEvents,
                                                          NULL);
        wiced_hal_mia_enable_mia_interrupt(WICED_TRUE);
        wiced_hal_mia_enable_lhl_interrupt(WICED_TRUE);

        /* Enable ghost' key detection */
        wiced_hal_keyscan_enable_ghost_detection(WICED_TRUE);

}

/*******************************************************************************
 * Function Name: kb_management_cback

 Function Description:
 @brief  Callback function that will be invoked by application_start()

 @param  event           Bluetooth management event type
 @param  p_event_data    Pointer to the the bluetooth management event data

 @return        Bluetooth status of the callback function
 ******************************************************************************/
static wiced_result_t
kb_management_cback(wiced_bt_management_evt_t event,
                    wiced_bt_management_evt_data_t *p_event_data)
{

    wiced_result_t result = WICED_BT_SUCCESS;

    switch(event)
    {
     /* Bluetooth  stack enabled */
    case BTM_ENABLED_EVT:

        /* Start of Keyboard application */
        kbapp_init();

        break;

    default:
        break;

    }

    return (result);
}

#ifdef LOW_POWER_ENABLE

/*******************************************************************************
 * Function Name: post_sleep_callback
 * @brief post_sleep_callback function is part of ePDS sleep configuration. It
 *        allows the user to configure the device after waking up from sleep.
 *        This function configures the supermuxable Keyscan columns after
 *        wake-up.
 *
 * @param restore_configuration
 ******************************************************************************/
static void post_sleep_callback (wiced_bool_t restore_configuration)
{
    /* Configure number of rows and columns for keyscan */
    wiced_hal_keyscan_configure(NUM_KEYSCAN_ROWS, NUM_KEYSCAN_COLS);

    /* Mapping Keyscan columns to LHL GPIOs */
    kbapp_map_columns();

    /* Initialize keyscan */
    wiced_hal_keyscan_init();

}

/*******************************************************************************
 Function Name: keyscane_configure_sleep

 Function Description:
 @brief  The function modifies wiced_sleep_config_t variable for this app to
         configure sleep and wakeup parameters and handlers.

 @param wiced_sleep_config_t   Sleep Configuration structure

 @return wiced_result_t
 ******************************************************************************/
wiced_result_t keyscan_configure_sleep(wiced_sleep_config_t app_sleep_config)
{

    wiced_result_t result = WICED_SUCCESS;

    app_sleep_config.sleep_mode               = WICED_SLEEP_MODE_NO_TRANSPORT;
    app_sleep_config.host_wake_mode           = WICED_SLEEP_WAKE_ACTIVE_HIGH;
    app_sleep_config.device_wake_mode         = WICED_SLEEP_WAKE_ACTIVE_LOW;
    app_sleep_config.device_wake_source       = WICED_SLEEP_WAKE_SOURCE_GPIO
                                              | WICED_SLEEP_WAKE_SOURCE_KEYSCAN
                                              | WICED_SLEEP_WAKE_SOURCE_QUAD;
    app_sleep_config.device_wake_gpio_num     = 255;
    app_sleep_config.sleep_permit_handler     = keyscan_sleep_handler;
    app_sleep_config.post_sleep_cback_handler = post_sleep_callback;

    result = wiced_sleep_configure(&app_sleep_config);

    return result;
}

/*******************************************************************************
 Function Name: keyscan_sleep_handler

 Function Description:
 @brief  This is a sleep permit handler function which polls the device hardware
         to know when to go to sleep.

 @param wiced_sleep_poll_type_t   Sleep poll type

 @return uint32_t Time to sleep
 ******************************************************************************/
uint32_t keyscan_sleep_handler(wiced_sleep_poll_type_t type)
{

    uint32_t sleep_control_flag = WICED_SLEEP_NOT_ALLOWED;

    WICED_BT_TRACE(".");

    switch(type)
    {

    case WICED_SLEEP_POLL_SLEEP_PERMISSION:
        /* Don't sleep when there is a active key press */
        if (keyscanActive())
        {
            WICED_BT_TRACE("$");
            sleep_control_flag = WICED_SLEEP_ALLOWED_WITHOUT_SHUTDOWN;
        }
        break;

    case WICED_SLEEP_POLL_TIME_TO_SLEEP:
        /*
         * Sleep when there is no active key press or when the keyscan block is
         * recovering
         */
        if(!keyscanActive())
        {
            sleep_control_flag = WICED_SLEEP_MAX_TIME_TO_SLEEP;
        }
        break;

    }

    return sleep_control_flag;
}
#endif

/*******************************************************************************
Function Name: application_start

Function Description:
 @brief Entry point of user Application - The user function
       that will be executed immediately after boot-up.

 @param void

 @return void
 ******************************************************************************/
void application_start(void)
{

    /*WICED_BT_TRACE_ENABLE*/
#if defined WICED_BT_TRACE_ENABLE || defined TRACE_TO_WICED_HCI
        wiced_set_debug_uart(WICED_ROUTE_DEBUG_TO_PUART);
#endif

    WICED_BT_TRACE("\r\n*****Keyscan Application Started!*****\r\n");

    wiced_bt_stack_init(kb_management_cback, NULL, NULL);

    /* Configure the device to enter sleep if the device is idle */
#ifdef LOW_POWER_ENABLE
    if(WICED_SUCCESS == keyscan_configure_sleep(app_sleep_config))
    {
        WICED_BT_TRACE("\r\nSleep configured successfully\r\n");
    }
#endif

}

