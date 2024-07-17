#include <driver/i2s.h>
#include <Arduino.h>
#include <audio_data.h>

#define I2S_WS  41 
#define I2S_SD  39
#define I2S_SCK 40

#define I2S_PORT I2S_NUM_0

#define BUFFER_LENGTH 512

void i2s_init()
{
  i2s_config_t i2s_config ={
    .mode= i2s_mode_t( I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate=16000,
    .bits_per_sample=i2s_bits_per_sample_t(16),
    .channel_format=I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format=i2s_comm_format_t(I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags=ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count=2,
    .dma_buf_len= BUFFER_LENGTH,
    .use_apll=false
    // .tx_desc_auto_clear=true,
    // .fixed_mclk=0
  };
  i2s_pin_config_t pin_config={
    .bck_io_num=I2S_SCK,
    .ws_io_num=I2S_WS,
    .data_out_num=I2S_SD,
    .data_in_num=I2S_PIN_NO_CHANGE
  };
  i2s_driver_install(I2S_PORT,&i2s_config,0,NULL);
  i2s_set_pin(I2S_PORT,&pin_config);

}
void play_audio();
int16_t* convert_audio_data(const uint8_t *audio_data, size_t audio_data_len);


void setup() {
  Serial.begin(115200);
  Serial.println("Multiplayer Started");

  delay(1000);
  i2s_init();


  play_audio();
}

void loop() {
}

void play_audio()
{
  size_t bytes_written;
  size_t offset=0;

  while(offset<audio_data_len * sizeof(uint8_t))
  {
    if (offset >= audio_data_len* sizeof(int8_t)-BUFFER_LENGTH) 
    {
      offset = 0;
    }

    size_t bytes_to_write=audio_data_len* sizeof(uint8_t)-offset;
    
    if (bytes_to_write>BUFFER_LENGTH)
    {
      bytes_to_write=BUFFER_LENGTH;
    }

    int16_t* converted_Data=convert_audio_data(&audio_data[offset],bytes_to_write*sizeof(int8_t));
    i2s_write(I2S_PORT,converted_Data,bytes_to_write,&bytes_written,portMAX_DELAY);
    offset+=bytes_written;
    free(converted_Data);
    vTaskDelay(pdMS_TO_TICKS(16));
  }

}


int16_t* convert_audio_data(const uint8_t *audio_data, size_t audio_data_len) {

  // Step 1: Convert from uint8_t to uint16_t
  int16_t *audio_data_uint16 = (int16_t *)malloc(audio_data_len / 2 * sizeof(uint16_t));
  for (size_t i = 0; i < audio_data_len / 2; i++) 
  {
    audio_data_uint16[i] = (audio_data[2 * i + 1] << 8) | audio_data[2 * i];
  }

  
  return audio_data_uint16;
}
