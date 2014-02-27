/**
 * Copyright (C) 2009-2013  Przemysław Dobrowolski
 *
 * This file is part of the Configuration Space Library (libcs), a library
 * for creating configuration spaces of various motion planning problems.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "compressor.h"

#if 0

// boost iostreams version

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

void Compressor::compress(const std::string &input, std::string &output)
{
    output.clear();
    boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
    //out.push(boost::iostreams::zlib_compressor());
    out.push(boost::iostreams::bzip2_compressor());
    out.push(boost::iostreams::back_inserter(output));
    boost::iostreams::copy(boost::make_iterator_range(input), out);
}

void Compressor::decompress(const std::string &input, std::string &output)
{
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    //in.push(boost::iostreams::zlib_decompressor());
    in.push(boost::iostreams::bzip2_decompressor());
    in.push(boost::make_iterator_range(input));
    output.clear();
    boost::iostreams::copy(in, boost::iostreams::back_inserter(output));
}

#else

// miniz version

#include "miniz.c"
#include <malloc.h>
#include <stdexcept>

void Compressor::compress(const std::string &input, std::string &output)
{
    // prepare output buffer
    mz_ulong destinationLength = mz_compressBound(static_cast<mz_ulong>(input.size()));
    unsigned char *destination = static_cast<unsigned char *>(malloc(destinationLength));

    if (!destination)
        throw std::runtime_error("out of memory!");

    // input buffer
    const unsigned char *source = reinterpret_cast<const unsigned char *>(input.c_str());
    mz_ulong sourceLength = static_cast<mz_ulong>(input.size());
    int level = 9;

    // compress
    int result = mz_compress2(destination, &destinationLength, source, sourceLength, level);

    if (result != MZ_OK)
        throw std::runtime_error("mz_compress2 failed!");

    // store result
    output = std::string(reinterpret_cast<char *>(&sourceLength), reinterpret_cast<char *>(&sourceLength) + sizeof(mz_ulong)) +
             std::string(reinterpret_cast<char *>(destination), reinterpret_cast<char *>(destination) + static_cast<size_t>(destinationLength));

    // free temporary data
    free(destination);
}

void Compressor::decompress(const std::string &input, std::string &output)
{
    // prepare output buffer
    mz_ulong destinationLength = *reinterpret_cast<const mz_ulong *>(input.c_str());
    unsigned char *destination = static_cast<unsigned char *>(malloc(destinationLength));

    if (!destination)
        throw std::runtime_error("out of memory!");

    // input buffer
    const unsigned char *source = reinterpret_cast<const unsigned char *>(input.c_str()) + sizeof(mz_ulong);
    mz_ulong sourceLength = static_cast<mz_ulong>(input.size() - sizeof(mz_ulong));

    // compress
    int result = mz_uncompress(destination, &destinationLength, source, sourceLength);

    if (result != MZ_OK)
        throw std::runtime_error("mz_uncompress failed!");

    // store result
    output = std::string(reinterpret_cast<char *>(destination), reinterpret_cast<char *>(destination) + static_cast<size_t>(destinationLength));

    // free temporary data
    free(destination);
}

#endif
