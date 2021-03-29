//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	WAD I/O functions.
//

#include <stdio.h>
#include <string.h>

#include "m_misc.h"
#include "m_argv.h"
#include "w_file.h"
#include "z_zone.h"
#include "i_system.h"
#include "deh_str.h"


// On KaiOS IndexedDB refuses to save files bigger than 16 Mb
// So we split the data into 8 Mb chunks

// 8 Mb * 32 = 256 Mb max WAD file size, biggest WAD is Damnation.wad, it's 250 Mb and cannot be loaded by Chocolate Doom
#define MAX_CHUNKS 32

typedef struct
{
    wad_file_t wad;
    int count;
    FILE *fstreams[MAX_CHUNKS];
} stdc_wad_file_chunked_t;

extern wad_file_class_t stdc_wad_file_chunked;

static wad_file_t *W_StdC_OpenFileChunked(char *path)
{
    stdc_wad_file_chunked_t *result;
    int count;
    FILE *fstream;
    long lastChunkSize = 0;

    fstream = fopen(path, "rb");

    if (fstream == NULL)
    {
        return NULL;
    }

    if (strlen(path) >= FILENAME_LIMIT)
    {
        I_Error("W_StdC_OpenFileChunked: WAD file name too long.");
        return NULL;
    }

    // Create a new stdc_wad_file_t to hold the file handle.

    result = Z_Malloc(sizeof(stdc_wad_file_chunked_t), PU_STATIC, 0);
    result->wad.file_class = &stdc_wad_file_chunked;
    result->wad.mapped = NULL;
    result->wad.path = M_StringDuplicate(path);

    result->fstreams[0] = fstream;
    lastChunkSize = M_FileLength(fstream);
    result->wad.length = lastChunkSize;
    if (lastChunkSize > FS_MAX_FILE_SIZE || lastChunkSize < 0)
    {
        I_Error("W_StdC_OpenFileChunked: %s is bigger than %ld bytes\n", path, lastChunkSize);
        return NULL;
    }
    //DEH_printf("Opened chunk %s size %ld\n", path, lastChunkSize);

    for (count = 1; count < MAX_CHUNKS; count++)
    {
        char chunkPath[FILENAME_LIMIT + 10];
        M_snprintf(chunkPath, sizeof(chunkPath), "%s.%d", path, count);
        fstream = fopen(chunkPath, "rb");

        if (fstream == NULL)
        {
            break;
        }

        if (lastChunkSize != FS_MAX_FILE_SIZE)
        {
            I_Error("W_StdC_OpenFileChunked: WAD chunk before %s is not exactly %ld bytes\n", chunkPath, lastChunkSize);
            return NULL;
        }

        result->fstreams[count] = fstream;
        lastChunkSize = M_FileLength(fstream);
        result->wad.length += lastChunkSize;
        if (lastChunkSize > FS_MAX_FILE_SIZE || lastChunkSize < 0)
        {
            I_Error("W_StdC_OpenFileChunked: %s is bigger than %ld bytes\n", chunkPath, lastChunkSize);
            return NULL;
        }
        //DEH_printf("Opened chunk %s size %ld\n", chunkPath, lastChunkSize);
    }

    result->count = count;
    //DEH_printf("Opened %s total size %d chunk count %d\n", path, result->wad.length, result->count);

    return &result->wad;
}

static void W_StdC_CloseFileChunked(wad_file_t *wad)
{
    stdc_wad_file_chunked_t *stdc_wad;
    int i;

    stdc_wad = (stdc_wad_file_chunked_t *) wad;

    for (i = 0; i < stdc_wad->count; i++)
    {
        fclose(stdc_wad->fstreams[i]);
    }
    Z_Free(stdc_wad);
}

// Read data from the specified position in the file into the 
// provided buffer.  Returns the number of bytes read.

size_t W_StdC_ReadChunked(wad_file_t *wad, unsigned int offset,
                   void *buffer, size_t buffer_len)
{
    stdc_wad_file_chunked_t *stdc_wad = (stdc_wad_file_chunked_t *) wad;
    size_t result = 0;

    if (offset >= stdc_wad->wad.length)
    {
        return 0;
    }
    if (offset + buffer_len > stdc_wad->wad.length)
    {
        buffer_len = stdc_wad->wad.length - offset;
    }

    unsigned int chunk = offset / FS_MAX_FILE_SIZE;
    unsigned int chunkEnd = (offset + buffer_len) / FS_MAX_FILE_SIZE;
    unsigned int offsetEnd = (offset + buffer_len) % FS_MAX_FILE_SIZE;
    offset = offset % FS_MAX_FILE_SIZE;

    while (chunk < chunkEnd)
    {
        fseek(stdc_wad->fstreams[chunk], offset, SEEK_SET);
        result += fread(buffer, 1, FS_MAX_FILE_SIZE - offset, stdc_wad->fstreams[chunk]);
        chunk++;
        offset = 0;
    }

    if (offset < offsetEnd)
    {
        fseek(stdc_wad->fstreams[chunk], offset, SEEK_SET);
        result += fread(buffer, 1, offsetEnd - offset, stdc_wad->fstreams[chunk]);
    }

    return result;
}

wad_file_class_t stdc_wad_file_chunked = 
{
    W_StdC_OpenFileChunked,
    W_StdC_CloseFileChunked,
    W_StdC_ReadChunked,
};
