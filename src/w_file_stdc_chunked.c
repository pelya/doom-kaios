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

#include "m_misc.h"
#include "w_file.h"
#include "z_zone.h"


// On KaiOS IndexedDB refuses to save files bigger than 16 Mb
// So we split the data into several small files

typedef struct
{
    wad_file_t wad;
    FILE *fstream;
} stdc_wad_file_chunked_t;

extern wad_file_class_t stdc_wad_file_chunked;

static wad_file_t *W_StdC_OpenFileChunked(char *path)
{
    stdc_wad_file_chunked_t *result;
    FILE *fstream;

    fstream = fopen(path, "rb");

    if (fstream == NULL)
    {
        return NULL;
    }

    // Create a new stdc_wad_file_t to hold the file handle.

    result = Z_Malloc(sizeof(stdc_wad_file_chunked_t), PU_STATIC, 0);
    result->wad.file_class = &stdc_wad_file_chunked;
    result->wad.mapped = NULL;
    result->wad.length = M_FileLength(fstream);
    result->wad.path = M_StringDuplicate(path);
    result->fstream = fstream;

    return &result->wad;
}

static void W_StdC_CloseFileChunked(wad_file_t *wad)
{
    stdc_wad_file_chunked_t *stdc_wad;

    stdc_wad = (stdc_wad_file_chunked_t *) wad;

    fclose(stdc_wad->fstream);
    Z_Free(stdc_wad);
}

// Read data from the specified position in the file into the 
// provided buffer.  Returns the number of bytes read.

size_t W_StdC_ReadChunked(wad_file_t *wad, unsigned int offset,
                   void *buffer, size_t buffer_len)
{
    stdc_wad_file_chunked_t *stdc_wad;
    size_t result;

    stdc_wad = (stdc_wad_file_chunked_t *) wad;

    // Jump to the specified position in the file.

    fseek(stdc_wad->fstream, offset, SEEK_SET);

    // Read into the buffer.

    result = fread(buffer, 1, buffer_len, stdc_wad->fstream);

    return result;
}


wad_file_class_t stdc_wad_file_chunked = 
{
    W_StdC_OpenFileChunked,
    W_StdC_CloseFileChunked,
    W_StdC_ReadChunked,
};
