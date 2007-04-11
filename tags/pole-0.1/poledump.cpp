/* POLE - Portable library to access OLE Storage 
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, US
*/

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <list>
#include <string>

#include "pole.h"

void visit( int indent, POLE::Storage* storage )
{
  std::list<std::string> entries;
  entries = storage->listDirectory();

  std::list<std::string>::iterator it;
  for( it = entries.begin(); it != entries.end(); ++it )
  {
    std::string n = *it;
    for( int j = 0; j < indent; j++ ) std::cout << "    ";
    std::cout << n << std::endl;
    if( storage->enterDirectory( n ) )
    {
      visit( indent+1, storage );
      storage->leaveDirectory();
    }
  }
  
}

void dump( POLE::Storage* storage, char* stream_name )
{
  POLE::Stream* stream;
  stream = storage->stream( stream_name );
  if( !stream ) return;
  
  // std::cout << "Size: " << stream->size() << " bytes" << std::endl;
  unsigned char buffer[16];
  for( ;; )
  {
      unsigned read = stream->read( buffer, sizeof( buffer ) );
      for( unsigned i = 0; i < read; i++ )
        printf( "%02x ", buffer[i] );
      std::cout << "    ";
      for( unsigned i = 0; i < read; i++ )
        printf( "%c", (buffer[i]>=32) ? buffer[i] : '.' );
      std::cout << std::endl;      
      if( read < sizeof( buffer ) ) break;
  }
}

void extract( POLE::Storage* storage, char* stream_name, char* outfile )
{
  POLE::Stream* stream;
  stream = storage->stream( stream_name );
  if( !stream ) return;
  
  std::ofstream file;
  file.open( outfile, std::ios::binary|std::ios::out );
  
  unsigned char buffer[16];
  for( ;; )
  {
      unsigned read = stream->read( buffer, sizeof( buffer ) );
      file.write( (const char*)buffer, read  );
      if( read < sizeof( buffer ) ) break;
  }

  file.close();
}

int main(int argc, char *argv[])
{
  if( argc < 2 )
  {
    std::cout << "Usage:" << std::endl;
    std::cout << argv[0] << " filename [stream-name [output-file]]" << std::endl;
    return 0;
  }
  
  char* filename = argv[1];
  char* streamname = (argc<3) ? 0 : argv[2];
  char* outfile = (argc<4) ? 0 : argv[3];

  POLE::Storage* storage = new POLE::Storage( filename );
  storage->open();
  if( storage->result() != POLE::Storage::Ok )
  {
    std::cout << "Error on file " << filename << std::endl;
    return 1;
  }
  
  if( !streamname )
    visit( 0, storage );
  else if( !outfile )
     dump( storage, streamname );
  else
     extract( storage, streamname, outfile );
    
  delete storage;
  
  return 0;
}
