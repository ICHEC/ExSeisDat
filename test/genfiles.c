#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Write a file named \c filename of size \c file_size with the repeating
/// pattern \c pattern
/// @param[in] filename     The name of the file to write
///                         (null terminated string)
/// @param[in] file_size    The size of the file to write
/// @param[in] pattern      The pattern to write to the file
///                         (pointer to array of size \c pattern_size)
/// @param[in] pattern_size The size of the \c pattern array
void make_file(
    const char* filename,
    size_t file_size,
    const unsigned char* pattern,
    size_t pattern_size)
{
    FILE* file = fopen(filename, "w");

    // If file_size is 0, just close the empty file and return
    if (file_size == 0) {
        fclose(file);
        return;
    }

    // Make sure we're at the start of the file
    fseek(file, 0U, SEEK_SET);

    // Find out how many full patterns we have, and how much extra needs to
    // be added to fill the file.
    const size_t pattern_reps = file_size / pattern_size;
    const size_t padding_size = file_size % pattern_size;

    // Fill the full patterns
    for (size_t i = 0; i < pattern_reps; i++) {
        fwrite(pattern, sizeof(unsigned char), pattern_size, file);
    }

    // Fill the remaining space (if any)
    if (padding_size != 0) {
        fwrite(pattern, sizeof(unsigned char), padding_size, file);
    }

    // Done.
    fclose(file);
}


// Functions for the trace parameters of the generated SEGY file

/// Function to generate the in-line trace parameter for trace \c i
/// @param[in] trace_num The id of the trace to generate the in-line value for.
int32_t il_num(size_t trace_num)
{
    return 1600L + (trace_num / 3000L);
}

/// Function to generate the cross-line trace parameter for trace \c i
/// @param[in] trace_num The id of the trace to generate the cross-line value
///                      for.
int32_t xl_num(size_t trace_num)
{
    return 1600L + (trace_num % 3000L);
}

/// Function to generate the source x coordinate trace parameter for trace \c i
/// @param[in] trace_num The id of the trace to generate the source x coordinate
///                      value for.
int32_t x_num(size_t trace_num)
{
    return 1000L + (trace_num / 2000L);
}

/// Function to generate the source y coordinate trace parameter for trace \c i
/// @param[in] trace_num The id of the trace to generate the source y coordinate
///                      value for.
int32_t y_num(size_t trace_num)
{
    return 1000L + (trace_num % 2000L);
}

/// Helper function, find the minimum of \c a and \b.
/// @param[in] a A value to compare
/// @param[in] b A value to compare
static size_t min(size_t a, size_t b)
{
    if (a < b) {
        return a;
    }
    return b;
}

/// Write a SEGY file to filename with \c ns samples per trace and \c nt traces,
/// using a maximum of \c maxBlock memory while writing.
/// @param[in] filename The name of the file to write to
/// @param[in] ns       The number of samples per trace
/// @param[in] nt       The number of traces in the SEGY file
/// @param[in] maxBlock The maximum amount of memory to use for writing.
void make_segy(const char* filename, size_t ns, size_t nt, size_t max_block)
{
    const size_t header_size       = 3600;
    const size_t trace_header_size = 240U;
    const size_t data_object_size  = (trace_header_size + ns * sizeof(float));

    const size_t file_size =
        header_size + (trace_header_size + ns * sizeof(float)) * nt;

    FILE* file = fopen(filename, "w");

    // If you are trapped here, make sure you created the required folders
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file: %s", filename);
        exit(EXIT_FAILURE);
    }

    // If we have traces to write, not just the file header
    if (file_size >= header_size) {

        // Set the file headers

        // Sample interval
        int16_t inc            = 20;
        unsigned char cinc[2U] = {(inc & 0xFF00) >> 8U, inc & 0xFF};
        fseek(file, 3217U - 1, SEEK_SET);
        fwrite(cinc, sizeof(unsigned char), 2U, file);

        // Sample data format code (5 = 4-byte IEEE)
        unsigned char format = 5;
        fseek(file, 3225U - 1, SEEK_SET);
        fwrite(&format, sizeof(unsigned char), 1U, file);

        // Number of samples per data trace
        unsigned char cns[2U] = {(ns & 0xFF00) >> 8U, ns & 0xFF};
        fseek(file, 3221U - 1, SEEK_SET);
        fwrite(cns, sizeof(unsigned char), 2U, file);

        // Move to start of file body
        fseek(file, 3600U, SEEK_SET);
        unsigned char zero = 0;
        fwrite(&zero, 1, 1, file);
        fseek(file, 3600U, SEEK_SET);

        // Find write buffer size and number of traces per write buffer,
        // up to a maximum of maxBlock.
        size_t buffer_size = min(max_block, (file_size - header_size));
        buffer_size -= buffer_size % data_object_size;
        size_t nt_per_buffer = buffer_size / data_object_size;

        // The buffer we'll be filling and writing to disk
        unsigned char* buffer = calloc(buffer_size, sizeof(unsigned char));

        // Write the SEGY file
        for (size_t i = 0; i < nt; i += nt_per_buffer) {

            // Find the amount of data to be written on this iteration
            size_t chunk = min(nt - i, nt_per_buffer);

            // Loop over the traces to be built/written
            #pragma omp parallel for
            for (size_t j = 0; j < chunk; j++) {

                // A pointer to where this trace starts in the buffer
                unsigned char* trace_buffer = &buffer[data_object_size * j];

                // Write the trace data
                for (size_t k = 0; k < ns; k++) {

                    const float f = i + j + k;
                    uint32_t f_i  = 0;
                    memcpy(&f_i, &f, sizeof(uint32_t));

                    trace_buffer[trace_header_size + 4 * k + 0] =
                        f_i >> 24 & 0xFF;
                    trace_buffer[trace_header_size + 4 * k + 1] =
                        f_i >> 16 & 0xFF;
                    trace_buffer[trace_header_size + 4 * k + 2] =
                        f_i >> 8 & 0xFF;
                    trace_buffer[trace_header_size + 4 * k + 3] = f_i & 0xFF;
                }


                // Write the trace headers

                // Coordinate scaler
                int16_t scale    = 1;
                trace_buffer[70] = scale >> 8 & 0xFF;
                trace_buffer[71] = scale & 0xFF;

                // Source coordinate - X
                int32_t x        = x_num(i + j);
                trace_buffer[72] = x >> 24 & 0xFF;
                trace_buffer[73] = x >> 16 & 0xFF;
                trace_buffer[74] = x >> 8 & 0xFF;
                trace_buffer[75] = x & 0xFF;

                // Source coordinate - Y
                int32_t y        = y_num(i + j);
                trace_buffer[76] = y >> 24 & 0xFF;
                trace_buffer[77] = y >> 16 & 0xFF;
                trace_buffer[78] = y >> 8 & 0xFF;
                trace_buffer[79] = y & 0xFF;

                // In-line number
                int32_t il        = il_num(i + j);
                trace_buffer[188] = il >> 24 & 0xFF;
                trace_buffer[189] = il >> 16 & 0xFF;
                trace_buffer[190] = il >> 8 & 0xFF;
                trace_buffer[191] = il & 0xFF;

                // Cross-line number
                int32_t xl        = xl_num(i + j);
                trace_buffer[192] = xl >> 24 & 0xFF;
                trace_buffer[193] = xl >> 16 & 0xFF;
                trace_buffer[194] = xl >> 8 & 0xFF;
                trace_buffer[195] = xl & 0xFF;
            }

            // Write the buffer to disk
            fwrite(
                buffer, sizeof(unsigned char), chunk * data_object_size, file);
        }

        // Cleanup the buffer and file
        free(buffer);
        fclose(file);
    }
}

int main(int argc, char* argv[])
{
    const size_t pattern_size = 0x100;
    unsigned char* pattern    = malloc(pattern_size * sizeof(unsigned char));

    // Set the pattern
    for (size_t i = 0; i < pattern_size; i++) {
        pattern[i] = i % pattern_size;
        // pattern[i] = i + i % 3 + i % 9 + i % (pattern_size - 7);
    }

    // Loop over the command line arguments.
    for (int i = 1; i < argc; i++) {

        //
        // Files for unit tests
        //

        const char* filename = argv[i];

        // Find the base name of the file
        const char* base_name = filename + strlen(filename);
        while (base_name != filename) {
            base_name--;
            if (*base_name == '/' || *base_name == '\\') {
                base_name++;
                break;
            }
        }

        printf("Generating file: %s\n", filename);

        if (strcmp("small_file_pattern.tmp", base_name) == 0) {
            make_file(filename, 4096ll, pattern, pattern_size);
            continue;
        }
        if (strcmp("large_file_pattern.tmp", base_name) == 0) {
            make_file(
                filename, 10ll * 1024ll * 1024ll * 1024ll, pattern,
                pattern_size);
            continue;
        }
        if (strcmp("small_segy.tmp", base_name) == 0) {
            make_segy(filename, 261U, 400U, 1024U * 1024U);
            continue;
        }
        if (strcmp("big_trace_segy.tmp", base_name) == 0) {
            make_segy(filename, 32000U, 40000U, 1024U * 1024U);
            continue;
        }
        if (strcmp("large_segy.tmp", base_name) == 0) {
            make_segy(filename, 1000U, 2000000U, 1024U * 1024U);
            continue;
        }


        //
        // Files for system tests
        //

        if (strcmp("no_trace.segy", base_name) == 0) {
            make_segy(filename, 1000U, 0U, 3600U);
            continue;
        }
        if (strcmp("one_trace.segy", base_name) == 0) {
            make_segy(filename, 1000U, 1U, 1024U * 1024U * 1024U);
            continue;
        }
        if (strcmp("one_big_trace.segy", base_name) == 0) {
            make_segy(filename, 32768U, 1U, 16U * 1024U * 1024U);
            continue;
        }
        if (strcmp("small_segy.segy", base_name) == 0) {
            make_segy(filename, 1000U, 2200000U, 1024U * 1024U * 1024U);
            continue;
        }


        // Uncaught value for argv
        printf("Error! Unknown filename %s!\n", filename);
        free(pattern);
        return EXIT_FAILURE;
    }

    free(pattern);
    return EXIT_SUCCESS;
}
