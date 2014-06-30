# ncxmms2

## About
ncxmms2 is an ncurses xmms2 client. This project is inspired by
[ncmpcpp](http://ncmpcpp.rybczak.net/) and aimed to have similiar look and feel.

## Status
The project is still under development but already in pretty usable state.
Currently supported features:
 * Base features:
   * Display playback status
   * Playback control
 
  
 * Screens:
  * Help
  * Active playlist
  * Local file system browser
  * Medialib browser
  * Playlists browser
  * Equalizer

## Key bindings
Screens switching:
 * `1` - Help 
 * `2` - Active playlist
 * `3` - Local file system browser
 * `5` - Medialib browser
 * `6` - Playlists browser
 * `7` - Equalizer
  
Playback control:
 * `P` - Toggle
 * `s` - Stop
 * `>` - Next
 * `<` - Prev
 * `f` - Seek forward
 * `b` - Seek backward

List view:
 * `up arrow` - Move cursor up
 * `down arrow` - Move cursor down
 * `Home` - Move cursor to the first item
 * `End` - Move cursor to the last item
 * `insert` - Toggle selection
 * `*` - Invert selection
 * `+` - Select items by regular expression
 * `\` - Unselect items by regular expression
 * `.` - Jump to next selected item
 * `,` - Jump to previous selected item
  
Playlists browser:
 * `n` - Create new playlist
 * `Delete` - Remove playlist
 * `r` - Rename playlist
 * `o` - Go to currently active playlist
  
Playlist view:
 * `c` - Clear playlist
 * `S` - Shuffle playlist
 * `o` - Go to currently playing song
 * `m` - Move selected songs
 * `Ctrl + O` - Add file / directory
 * `Ctrl + U` - Add url
 * `i` - Show song info
  
Local file system browser:
 * `space` - Add file or directory to active playlist
 * `enter` - Add file to active playlist
 * `left arrow` - Go up
 * `g` - Change directory
 * `R` - Reload directory
 
Medialib browser:
 * `R` - Refresh
 * `space`, `enter` - Add artist / album / song to active playlist
 * `i` - Show song info

## Settings
For customization see ncxmms2.conf and ncxmms2.colors.