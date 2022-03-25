/**
 * \file device.c
 * \brief Benchmark #1.1 CPU version (sequential) device initialization. 
 * \author Ivan Rodriguez-Ferrandez (BSC)
 */
#include "device.h"
#include "processing.h"

void init(
	image_data_t *image_data,
	image_time_t *t,
	char *device_name
	)
{
    init(image_data,t, 0,0, device_name);
}



void init(
	image_data_t *image_data,
	image_time_t *t,
	int platform,
	int device,
	char *device_name
	)
{
    // TBD Feature: device name. -- Bulky generic platform implementation
	strcpy(device_name,"Generic device");

	/* Time object init */
	t->t_frame =  (time_t*)malloc(sizeof(time_t) * image_data->num_frames); 
	t->t_offset =  (time_t*)malloc(sizeof(time_t) * image_data->num_frames); 
	t->t_badpixel =  (time_t*)malloc(sizeof(time_t) * image_data->num_frames); 
	t->t_scrub =  (time_t*)malloc(sizeof(time_t) * image_data->num_frames); 
	t->t_gain =  (time_t*)malloc(sizeof(time_t) * image_data->num_frames); 
	t->t_binning =  (time_t*)malloc(sizeof(time_t) * image_data->num_frames); 
	t->t_coadd =  (time_t*)malloc(sizeof(time_t) * image_data->num_frames); 
}


bool device_memory_init(
	image_data_t* image_data,
	frame16_t* input_frames,
	frame16_t* offset_map, 
	frame8_t* bad_pixel_map, 
	frame16_t* gain_map,
	unsigned int w_size,
	unsigned int h_size
	)
{	
	unsigned int frame_i;

	/* image_data_t memory allocation */
	image_data->frames = (frame16_t*)malloc(sizeof(frame16_t)* image_data->num_frames);
	for(frame_i=0; frame_i < image_data->num_frames; frame_i++)
	{
		image_data->frames[frame_i].f = (uint16_t*)malloc(input_frames->h * input_frames->w * sizeof(uint16_t));
	}
	image_data->offsets.f = (uint16_t*)malloc(sizeof(uint16_t) * offset_map->h * offset_map->w);
	image_data->gains.f = (uint16_t*)malloc(gain_map->h * gain_map->w * sizeof(uint16_t));
	image_data->bad_pixels.f = (uint8_t*)malloc(bad_pixel_map->h * bad_pixel_map->w * sizeof(uint8_t));

	image_data->image_output.f = (uint32_t*)malloc(w_size * h_size * sizeof(uint32_t));
	image_data->image_output.w = w_size;
	image_data->image_output.h = h_size;

	image_data->binned_frame.f = (uint32_t*)malloc(h_size * w_size * sizeof(uint32_t));
	image_data->binned_frame.w = w_size;
	image_data->binned_frame.h = h_size;
	
	

    return true;
}

void copy_memory_to_device(
	image_data_t *image_data,
	image_time_t *t,
	frame16_t *input_frames,
	frame16_t *correlation_table,
	frame16_t *gain_correlation_map,
	frame8_t *bad_pixel_map
	)
{
    
	unsigned int frame_i;

	/* image_data_t memory allocation */
	for(frame_i=0; frame_i < image_data->num_frames; frame_i++)
	{
		memcpy(image_data->frames[frame_i].f, input_frames[frame_i].f, sizeof(uint16_t) * input_frames[frame_i].h * input_frames[frame_i].w);
		image_data->frames[frame_i].h = input_frames[frame_i].h;
		image_data->frames[frame_i].w = input_frames[frame_i].w;
	}
	
	
	memcpy(image_data->offsets.f, correlation_table->f, sizeof(uint16_t) * correlation_table->h * correlation_table->w);
	image_data->offsets.h = correlation_table->h;
	image_data->offsets.w = correlation_table->w;

    memcpy(image_data->gains.f, gain_correlation_map->f, sizeof(uint16_t) * gain_correlation_map->h * gain_correlation_map->w);
	image_data->gains.h = gain_correlation_map->h;
	image_data->gains.w = gain_correlation_map->w;

    memcpy(image_data->bad_pixels.f, bad_pixel_map->f, sizeof(uint8_t) * bad_pixel_map->h * bad_pixel_map->w);
	image_data->bad_pixels.h = bad_pixel_map->h;
	image_data->bad_pixels.w = bad_pixel_map->w;
}


void process_benchmark(
	image_data_t *image_data,
	image_time_t *t,
	frame16_t *input_frames,
	unsigned int width,
    unsigned int height
	)
{    
    
	/* input frame, width and height is not use in the sequential version */

    unsigned int frame_i;
	static unsigned int offset_neighbours = 2;
	
	/* Loop through each frames and perform pre-processing. */
	T_START(t->t_test);
	for(frame_i=offset_neighbours; frame_i < image_data->num_frames - offset_neighbours; frame_i++)
	{
		// FIXME  Verify that the new format for the timing is correct
		
		T_START(t->t_frame[frame_i]);
		/*  Start the preparation of the frame, frame_i + 2, to be ready for the radiation scrubbing. */
		prepare_image_frame(image_data, t, &image_data->frames[frame_i + 2], frame_i + 2);
		/* Then compute the frame_i using the already calculate data from frame_i -2, frame_i -1, frame_i + 1 and frame_i + 2 */
		proc_image_frame(image_data, t, &image_data->frames[frame_i], frame_i);
		T_STOP(t->t_frame[frame_i]);
		

	}
	T_STOP(t->t_test);

}

void copy_memory_to_host(
	image_data_t *image_data,
	image_time_t *t,
	frame32_t *output_image
	)
{
    memcpy(output_image->f, image_data->image_output.f, sizeof(uint32_t) * image_data->image_output.h * image_data->image_output.w);
	output_image->h = image_data->image_output.h;
	output_image->w = image_data->image_output.w;
}


void get_elapsed_time(
	image_data_t *image_data, 
	image_time_t *t, 
	bool csv_format,
	bool full_time_output
	)
{	

	if (csv_format)
	{
		double elapsed_time =   (t->t_test) / ((double)(CLOCKS_PER_SEC / 1000)); 
		printf("%.10f;%.10f;%.10f;\n", (float) 0, elapsed_time, (float) 0);
	}
	else if (full_time_output)
	{
		static unsigned int offset_neighbours = 2;
		double elapsed_time_test =   (t->t_test) / ((double)(CLOCKS_PER_SEC / 1000)); 
		// print the time
		printf("Elapsed time test: %.10f miliseconds\n", elapsed_time_test);
		// now get each time per frame
		printf("############################################################\n");
		for (int i = offset_neighbours; i < image_data->num_frames - offset_neighbours; ++i)
		{	
			// print frame number
			printf("Frame %d: ", i);
			double elapsed_time_frame =   (t->t_frame[i]) / ((double)(CLOCKS_PER_SEC / 1000)); 
			printf("Elapsed time frame %d: %.10f miliseconds\n", i, elapsed_time_frame);
			// now get each time per each kernel
			/* [I]: Bias offset correction */ 
			double elapsed_time_offset =   (t->t_offset[i]) / ((double)(CLOCKS_PER_SEC / 1000));
			printf("Elapsed time offset %d: %.10f miliseconds\n", i, elapsed_time_offset);
			/* [II]: Bad pixel correction */
			double elapsed_time_t_badpixel =   (t->t_badpixel[i]) / ((double)(CLOCKS_PER_SEC / 1000));
			printf("Elapsed time badpixel %d: %.10f miliseconds\n", i, elapsed_time_t_badpixel);
			/* [III]: Radiation scrubbing */
			double elapsed_time_t_scrub =   (t->t_scrub[i]) / ((double)(CLOCKS_PER_SEC / 1000));
			printf("Elapsed time radiation scrubbing %d: %.10f miliseconds\n", i, elapsed_time_t_scrub);
			/* [IV]: Gain correction */
			double elapsed_time_t_gain =   (t->t_gain[i]) / ((double)(CLOCKS_PER_SEC / 1000));
			printf("Elapsed time gain %d: %.10f miliseconds\n", i, elapsed_time_t_gain);
			/* [V]: Spatial binning */
			double elapsed_time_t_binning =   (t->t_binning[i]) / ((double)(CLOCKS_PER_SEC / 1000));
			printf("Elapsed time binning %d: %.10f miliseconds\n", i, elapsed_time_t_binning);
			/* [VI]: Co-adding frames */
			double elapsed_time_t_coadd =   (t->t_coadd[i]) / ((double)(CLOCKS_PER_SEC / 1000));
			printf("Elapsed time coadd %d: %.10f miliseconds\n", i, elapsed_time_t_coadd);
			// print separator
			printf("############################################################\n");

		}
		
		printf("\n");
	}
}


void clean(
	image_data_t *image_data,
	image_time_t *t
	)
{
	unsigned int frame_i;

	/* Clean time */
	free(t->t_frame);
	free(t->t_offset);
	free(t->t_badpixel);
	free(t->t_scrub);
	free(t->t_gain);
	free(t->t_binning);
	free(t->t_coadd);
	free(t);

	for(frame_i=0; frame_i < image_data->num_frames; frame_i++)
	{
		free(image_data->frames[frame_i].f);
	}
	free(image_data->frames);
	free(image_data->offsets.f);
	free(image_data->gains.f);
	free(image_data->bad_pixels.f);
	free(image_data->scrub_mask.f);
	free(image_data->binned_frame.f);
    free(image_data->image_output.f);
	free(image_data);
}
