/*
 * Proland: a procedural landscape rendering library.
 * Website : http://proland.inrialpes.fr/
 * Copyright (c) 2008-2015 INRIA - LJK (CNRS - Grenoble University)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors 
 * may be used to endorse or promote products derived from this software without 
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * Proland is distributed under the Berkeley Software Distribution 3 Licence. 
 * For any assistance, feedback and enquiries about training programs, you can check out the 
 * contact page on our website : 
 * http://proland.inrialpes.fr/
 */
/*
 * Main authors: Eric Bruneton, Antoine Begault, Guillaume Piolat.
 */

/*
--------------------------------------------------------------------------------
-   Module      :   mem_file.c
-   Description :   A general purpose library for manipulating a memory area
-                   as if it were a file.
-                   mfs_ stands for memory file system.
-   Author      :   Mike Johnson - Banctec AB 03/07/96
-
--------------------------------------------------------------------------------
*/

/*

Copyright (c) 1996 Mike Johnson
Copyright (c) 1996 BancTec AB

Permission to use, copy, modify, distribute, and sell this software
for any purpose is hereby granted without fee, provided
that (i) the above copyright notices and this permission notice appear in
all copies of the software and related documentation, and (ii) the names of
Mike Johnson and BancTec may not be used in any advertising or
publicity relating to the software.

THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.

IN NO EVENT SHALL MIKE JOHNSON OR BANCTEC BE LIABLE FOR
ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
OF THIS SOFTWARE.

*/


/*
--------------------------------------------------------------------------------
-   Includes
--------------------------------------------------------------------------------
*/

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <errno.h>

#include "proland/util/mfs.h"

/*
--------------------------------------------------------------------------------
-   Function code
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
-   Function    :   extend_mem_file ()
-
-   Arguments   :   File descriptor, length to extend to.
-
-   Returns     :   0 - All OK, -1 - realloc () failed.
-
-   Description :   Increase the amount of memory allocated to a file.
-
--------------------------------------------------------------------------------
*/

static int extend_mem_file (mfs_file *fd, int size)
{
    void *new_mem;
    int ret;

    if ((new_mem = realloc (fd->buf, size)) == (void *) NULL)
        ret = -1;
    else
    {
        fd->buf = (char *) new_mem;
        ret = 0;
    }

    return (ret);
}

/*
--------------------------------------------------------------------------------
-   Function    :   mfs_open ()
-
-   Arguments   :   Pointer to allocated buffer, initial size of buffer,
-                   mode spec (r, w, a)
-
-   Returns     :   File descriptor or -1 if error.
-
-   Description :   Register this area of memory (which has been allocated
-                   and has a file read into it) under the mem_file library.
-                   A file descriptor is returned which can the be passed
-                   back to TIFFClientOpen and used as if it was a disk
-                   based fd.
-                   If the call is for mode 'w' then pass (void *)NULL as
-                   the buffer and zero size and the library will
-                   allocate memory for you.
-                   If the mode is append then pass (void *)NULL and size
-                   zero or with a valid address.
-
--------------------------------------------------------------------------------
*/

int mfs_open (void *buffer, int size, char *mode, mfs_file *fd)
{
    int ret;
    void *tmp;

    ret = 0;
    fd->buf_open = false;

    if (*mode == 'r')
    {
        if (buffer == (void *)NULL)
        {
            ret = -1;
            errno = EINVAL;
        }
        else
        {
            fd->buf = (char *)buffer;
            fd->buf_size = size;
            fd->buf_off = 0;
        }
    }
    else if (*mode == 'w')
    {

        if (buffer != (void *)NULL)
        {
            ret = -1;
            errno = EINVAL;
        }

        else
        {
            tmp = malloc (0);   /* Get a pointer */
            if (tmp == (void *)NULL)
            {
                ret = -1;
                errno = ENOMEM;
            }
            else
            {
                fd->buf = (char *)tmp;
                fd->buf_size = 0;
                fd->buf_off = 0;
            }
        }
    }
    else if (*mode == 'a')
    {
        if (buffer == (void *) NULL)    /* Create space for client */
        {
            tmp = malloc (0);   /* Get a pointer */
            if (tmp == (void *)NULL)
            {
                ret = -1;
                errno = ENOMEM;
            }
            else
            {
                fd->buf = (char *)tmp;
                fd->buf_size = 0;
                fd->buf_off = 0;
            }
        }
        else                            /* Client has file read in already */
        {
            fd->buf = (char *)buffer;
            fd->buf_size = size;
            fd->buf_off = 0;
        }
    }
    else        /* Some other invalid combination of parameters */
    {
        ret = -1;
        errno = EINVAL;
    }

    if (ret != -1)
    {
        fd->buf_mode = *mode;
        fd->buf_open = true;
    }

    return (ret);
}

/*
--------------------------------------------------------------------------------
-   Function    :   mfs_lseek ()
-
-   Arguments   :   File descriptor, offset, whence
-
-   Returns     :   as per man lseek (2)
-
-   Description :   Does the same as lseek (2) except on a memory based file.
-                   Note: the memory area will be extended if the caller
-                   attempts to seek past the current end of file (memory).
-
--------------------------------------------------------------------------------
*/

int mfs_lseek (mfs_file *fd, int offset, int whence)
{
    int ret;
    long test_off;

    if (!fd->buf_open)  /* Not open */
    {
        ret = -1;
        errno = EBADF;
    }
    else if (offset < 0 && whence == SEEK_SET)
    {
        ret = -1;
        errno = EINVAL;
    }
    else
    {
        switch (whence)
        {
            case SEEK_SET:
                if (offset > fd->buf_size)
                    extend_mem_file (fd, offset);
                fd->buf_off = offset;
                ret = offset;
                break;

            case SEEK_CUR:
                test_off = fd->buf_off + offset;

                if (test_off < 0)
                {
                    ret = -1;
                    errno = EINVAL;
                }
                else
                {
                    if (test_off > fd->buf_size)
                        extend_mem_file (fd, test_off);
                    fd->buf_off = test_off;
                    ret = test_off;
                }
                break;

            case SEEK_END:
                test_off = fd->buf_size + offset;

                if (test_off < 0)
                {
                    ret = -1;
                    errno = EINVAL;
                }
                else
                {
                    if (test_off > fd->buf_size)
                        extend_mem_file (fd, test_off);
                    fd->buf_off = test_off;
                    ret = test_off;
                }
                break;

            default:
                errno = EINVAL;
                ret = -1;
                break;
        }
    }

    return (ret);
}

/*
--------------------------------------------------------------------------------
-   Function    :   mfs_read ()
-
-   Arguments   :   File descriptor, buffer, size
-
-   Returns     :   as per man read (2)
-
-   Description :   Does the same as read (2) except on a memory based file.
-                   Note: An attempt to read past the end of memory currently
-                   allocated to the file will return 0 (End Of File)
-
--------------------------------------------------------------------------------
*/

int mfs_read (mfs_file *fd, void *clnt_buf, int size)
{
    int ret;

    if (!fd->buf_open || fd->buf_mode != 'r')
    {
        /* File is either not open, or not opened for read */

        ret = -1;
        errno = EBADF;
    }
    else if (fd->buf_off + size > fd->buf_size)
    {
        ret = 0;        /* EOF */
    }
    else
    {
        memcpy (clnt_buf, (void *) (fd->buf + fd->buf_off), size);
        fd->buf_off = fd->buf_off + size;
        ret = size;
    }

    return (ret);
}

/*
--------------------------------------------------------------------------------
-   Function    :   mfs_write ()
-
-   Arguments   :   File descriptor, buffer, size
-
-   Returns     :   as per man write (2)
-
-   Description :   Does the same as write (2) except on a memory based file.
-                   Note: the memory area will be extended if the caller
-                   attempts to write past the current end of file (memory).
-
--------------------------------------------------------------------------------
*/

int mfs_write (mfs_file *fd, void *clnt_buf, int size)
{
    int ret;

    if (!fd->buf_open || fd->buf_mode == 'r')
    {
        /* Either the file is not open or it is opened for reading only */

        ret = -1;
        errno = EBADF;
    }
    else if (fd->buf_mode == 'w')
    {
        /* Write */

        if (fd->buf_off + size > fd->buf_size)
        {
            extend_mem_file (fd, fd->buf_off + size);
            fd->buf_size = (fd->buf_off + size);
        }

        memcpy ((fd->buf + fd->buf_off), clnt_buf, size);
        fd->buf_off = fd->buf_off + size;

        ret = size;
    }
    else
    {
        /* Append */

        if (fd->buf_off != fd->buf_size)
            fd->buf_off = fd->buf_size;

        extend_mem_file (fd, fd->buf_off + size);
        fd->buf_size += size;

        memcpy ((fd->buf + fd->buf_off), clnt_buf, size);
        fd->buf_off = fd->buf_off + size;

        ret = size;
    }

    return (ret);
}

/*
--------------------------------------------------------------------------------
-   Function    :   mfs_size ()
-
-   Arguments   :   File descriptor
-
-   Returns     :   integer file size
-
-   Description :   This function returns the current size of the file in bytes.
-
--------------------------------------------------------------------------------
*/

int mfs_size (mfs_file *fd)
{
    int ret;

    if (!fd->buf_open)  /* Not open */
    {
        ret = -1;
        errno = EBADF;
    }
    else
        ret = fd->buf_size;

    return (ret);
}

/*
--------------------------------------------------------------------------------
-   Function    :   mfs_map ()
-
-   Arguments   :   File descriptor, ptr to address, ptr to length
-
-   Returns     :   Map status (succeeded or otherwise)
-
-   Description :   This function tells the client where the file is mapped
-                   in memory and what size the mapped area is. It is provided
-                   to satisfy the MapProc function in libtiff. It pretends
-                   that the file has been mmap (2)ped.
-
--------------------------------------------------------------------------------
*/

int mfs_map (mfs_file *fd, char **addr, size_t *len)
{
    int ret;

    if (!fd->buf_open)  /* Not open */
    {
        ret = -1;
        errno = EBADF;
    }
    else
    {
        *addr = fd->buf;
        *len = fd->buf_size;
        ret = 0;
    }

    return (ret);
}

/*
--------------------------------------------------------------------------------
-   Function    :   mfs_unmap ()
-
-   Arguments   :   File descriptor
-
-   Returns     :   UnMap status (succeeded or otherwise)
-
-   Description :   This function does nothing as the file is always
-                   in memory.
-
--------------------------------------------------------------------------------
*/

int mfs_unmap (mfs_file *fd)
{
    return (0);
}

/*
--------------------------------------------------------------------------------
-   Function    :   mfs_close ()
-
-   Arguments   :   File descriptor
-
-   Returns     :   close status (succeeded or otherwise)
-
-   Description :   Close the open memory file. (Make fd available again.)
-
--------------------------------------------------------------------------------
*/

int mfs_close (mfs_file *fd)
{
    int ret;

    if (!fd->buf_open)  /* Not open */
    {
        ret = -1;
        errno = EBADF;
    }
    else
    {
        fd->buf_open = false;
        ret = 0;
    }

    return (ret);
}
