/* gpio example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"

#include "sys/time.h"

#include "esp_log.h"
#include "esp_system.h"

SemaphoreHandle_t xMutex;
static const char *TAG = "main";

#define GPIO_OUTPUT_IO      2                               //Define output pin


static void wait_for_ms (uint32_t delay)
{
    uint32_t duration = delay/portTICK_RATE_MS;
    uint32_t begin = xTaskGetTickCount();
    uint32_t current = xTaskGetTickCount();

    while((current-begin)<duration){
        current = xTaskGetTickCount();
    }
    
}

static void gpio_level(void *arg)
{
    

    while(1) {
        ESP_LOGI(TAG, "GPIO Level: %d\n",gpio_get_level(GPIO_OUTPUT_IO));
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

static void gpio_ON(void*arg)
{
    while(1)
    {
        ESP_LOGI(TAG,"Trying high");
        xSemaphoreTake(xMutex,portMAX_DELAY);
        gpio_set_level(GPIO_OUTPUT_IO,1);
        ESP_LOGI(TAG,"GPIO set High\n");
        wait_for_ms(500);
        xSemaphoreGive(xMutex);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
    }
}

static void gpio_OFF(void*arg)
{
    while(1)
    {
        ESP_LOGI(TAG,"Trying low");
        xSemaphoreTake(xMutex,portMAX_DELAY);
        gpio_set_level(GPIO_OUTPUT_IO,0);
        ESP_LOGI(TAG,"GPIO set Low\n");
        wait_for_ms(500);
        xSemaphoreGive(xMutex);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
    }
}

void vApplicationIdleHook(void)
{                            
    ESP_LOGI(TAG,"Entering IDLE");
}

void app_main(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = (1ULL << GPIO_OUTPUT_IO);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    xMutex = xSemaphoreCreateMutex();
    if(xMutex!=NULL){
        ESP_LOGI(TAG,"Mutex Created\n");
    }

    //start gpio tasks
    xTaskCreate(gpio_ON,"gpio_ON",2048,NULL,2,NULL);
    xTaskCreate(gpio_OFF,"gpio_OFF",2048,NULL,1,NULL);
    xTaskCreate(gpio_level, "gpio_level", 2048, NULL, 3, NULL);

    while (1) {

    }
}


