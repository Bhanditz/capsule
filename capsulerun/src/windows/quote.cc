
/*
 *  capsule - the game recording and overlay toolkit
 *  Copyright (C) 2017, Amos Wenger
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details:
 * https://github.com/itchio/capsule/blob/master/LICENSE
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

// this godsend is brought to you by a neat little MSDN blog
// called "twisty little passages, all alike" - very appropriately so.
// https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/

#include "quote.h"

#include <string>

/**
 *     
 * Routine Description:
 *     
 *     This routine appends the given argument to a command line such
 *     that CommandLineToArgvW will return the argument string unchanged.
 *     Arguments in a command line should be separated by spaces; this
 *     function does not add these spaces.
 *     
 * Arguments:
 *     
 *     Argument - Supplies the argument to encode.
 * 
 *     CommandLine - Supplies the command line to which we append the encoded argument string.
 * 
 *     Force - Supplies an indication of whether we should quote
 *             the argument even if it does not contain any characters that would
 *             ordinarily require quoting.
 * 
 */
    
void ArgvQuote (
    const std::wstring& Argument,
    std::wstring& CommandLine,
    bool Force
)
  
{
    //
    // Unless we're told otherwise, don't quote unless we actually
    // need to do so --- hopefully avoid problems if programs won't
    // parse quotes properly
    //
    
    if (Force == false &&
        Argument.empty () == false &&
        Argument.find_first_of (L" \t\n\v\"") == Argument.npos)
    {
        CommandLine.append (Argument);
    }
    else {
        CommandLine.push_back (L'"');
        
        for (auto It = Argument.begin () ; ; ++It) {
            unsigned NumberBackslashes = 0;
        
            while (It != Argument.end () && *It == L'\\') {
                ++It;
                ++NumberBackslashes;
            }
        
            if (It == Argument.end ()) {
                
                //
                // Escape all backslashes, but let the terminating
                // double quotation mark we add below be interpreted
                // as a metacharacter.
                //
                
                CommandLine.append (NumberBackslashes * 2, L'\\');
                break;
            }
            else if (*It == L'"') {

                //
                // Escape all backslashes and the following
                // double quotation mark.
                //
                
                CommandLine.append (NumberBackslashes * 2 + 1, L'\\');
                CommandLine.push_back (*It);
            }
            else {
                
                //
                // Backslashes aren't special here.
                //
                
                CommandLine.append (NumberBackslashes, L'\\');
                CommandLine.push_back (*It);
            }
        }
    
        CommandLine.push_back (L'"');
    }
}