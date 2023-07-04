#ifndef __H_PCM8PP__
#define __H_PCM8PP__

/*
int32_t pcm8pp_play(int16_t channel, uint32_t mode, uint32_t size, uint32_t freq, void* addr);
int32_t pcm8pp_play_array_chain(int16_t channel, uint32_t mode, uint32_t count, uint32_t freq, void* addr);
int32_t pcm8pp_play_linked_array_chain(int16_t channel, uint32_t mode, uint32_t size, uint32_t freq, void* addr);
int32_t pcm8pp_play_ex_linked_array_chain(int16_t channel, uint32_t mode, uint32_t size, uint32_t freq, void* addr);
int32_t pcm8pp_set_channel_mode(int16_t channel, uint32_t mode, uint32_t freq);
int32_t pcm8pp_get_data_length(int16_t channel);
int32_t pcm8pp_get_channel_mode(int16_t channel);
int32_t pcm8pp_get_block_counter(int16_t channel);
int32_t pcm8pp_stop();
int32_t pcm8pp_pause();
int32_t pcm8pp_resume();
int32_t pcm8pp_set_max_channels(int16_t channels);
int32_t pcm8pp_set_polyphonic_mode(int16_t mode);
int32_t pcm8pp_set_frequency_mode(int16_t mode);
*/
int32_t pcm8pp_isavailable();

#endif