/*
  Winamp Control Plugin   version 1.0.1

  Copyright (c) 2003 x-phile
  All Rights Reserved.

  All used names are the property of their respective owners.
  Winamp is Copyright (c) by Nullsoft Inc.
  PowerPro and plugin template are Copyright (c) by Bruce Switzer.
*/

#include <windows.h>
#include <string.h>
#include <stdio.h>

//---DEFINITIONS----------------------------------------------------------------------------------------------------------------------------
#define GET_SONG_NAME                   29999 // Retrieves song's name
#define GET_SONG_NAME_AND_NUMBER        30000 // Gets song's name with number
#define GET_CAPTION                     30001 // Gets full Winamp's caption
#define GET_POSITION                    30002 // Returns current track position in miliseconds WM_USER id=105, data=0
#define GET_LENGTH                      30003 // Returns current track length in seconds WM_USER, id=105, data =1
#define SET_VOLUME                      30004 // Sets volume min = 0, max = 100
#define GET_SAMPLERATE                  30005 // Returns current track sample rate
#define GET_BITRATE                     30006 // Returns current track bitrate
#define GET_NUMBER_OF_CHANNELS          30007 // Returns current track number of channels
#define SHUFFLE_ON                      30008 // Turns shuffle on
#define SHUFFLE_OFF                     30009 // Turns shuffle off
#define REPEAT_ON                       30010 // Turns repeat on
#define REPEAT_OFF                      30011 // Turns repeat off
#define BLOCK_MINIBROWSER               30012 // Will block the Minibrowser from updates
#define UNBLOCK_MINIBROWSER             30013 // Will unblock the Minibrowser
#define SET_EQ_DATA                     30014 // Sets equalizer data min = -20 ; max = +20
#define GET_EQ_DATA                     30015 // Gets equalizer data min = -20 ; max = +20
#define GET_POSITION_IN_SECONDS         30016 // Gets current track position in seconds
#define SET_PANNING                     30017 // Sets the panning, which can be between -100 (all left) and 100 (all right).

//WM_USER
#define IPC_GETVERSION                  0   // Retrieves the version of Winamp running
#define IPC_DELETE                      101 // Clears Winamp's internal playlist.
#define IPC_STARTPLAY                   102 // Begins play of selected track.
#define IPC_ISPLAYING                   104 // Returns the status of playback.
#define IPC_GETOUTPUTTIME               105 // Gets length of track or position in miliseconds
#define IPC_JUMPTOTIME                  106 // Seeks within the current track.
#define IPC_WRITEPLAYLIST               120 // Writes out the current playlist to Winampdir\winamp.m3u
#define IPC_SETPLAYLISTPOS              121 // Sets the playlist position
#define IPC_SETVOLUME                   122 // Sets the volume, which can be between 0 (silent) and 255 (maximum).
#define IPC_SETPANNING                  123 // Sets the panning, which can be between -127 (all left) and 127 (all right).
#define IPC_GETLISTLENGTH               124 // Returns length of the current playlist, in tracks.
#define IPC_GETPOSITION                 125 // Returns the position in the current playlist, in tracks.
#define IPC_GETINFO                     126 // Retrieves info about the current playing track.
#define IPC_GETEQDATA                   127 // Retrieves one element of equalizer data.
#define IPC_SETEQDATA                   128 // Sets the value of the last position retrieved by IPC_GETEQDATA.
#define IPC_RESTART_WINAMP              135 // Restarts Winamp
#define IPC_INETAVAILABLE               242 // Returns 1 if the internet connecton is available for Winamp
#define IPC_UPDTITLE                    243 // Will ask Winamp to update the informations about the current title.
#define IPC_REFRESHPLCACHE              247 // Will flush the playlist cache buffer.
#define IPC_MBBLOCK                     248 // Will block the Minibrowser from updates if value is set to 1
#define IPC_GET_SHUFFLE                 250 // Returns the status of the Shuffle option (1 if set)
#define IPC_GET_REPEAT                  251 // Returns the status of the Repeat option (1 if set)
#define IPC_SET_SHUFFLE                 252 // Sets the status of the Shuffle option (1 to turn it on)
#define IPC_SET_REPEAT                  253 // Sets the status of the Repeat option (1 to turn it on)

//---NOT IMPLEMENTED------------------------------------------------------------------------------------------------------------------------
#define IPC_ADD_BOOKMARK                129 // Adds the specified file to the Winamp bookmark list
#define IPC_MBOPEN                      241 // Will open a new URL in the minibrowser. if url is NULL, it will open the Minibrowser window.
#define IPC_CHANGECURRENTFILE           245 // Will set the current playlist item.
#define IPC_GETMBURL                    246 // will retrieve the current Minibrowser URL
#define IPC_MBOPENREAL                  249 // Works the same as IPC_MBOPEN except that it will work even if IPC_MBBLOCK has been set to 1
//------------------------------------------------------------------------------------------------------------------------------------------

//WM_COPY_DATA
#define IPC_PLAYFILE                    100 // This will play specified file
#define IPC_CHDIR                       103 // This will make Winamp change to the specified directory
                                                                            
//WM_COMMAND
#define WINAMP_OPTIONS_EQ               40036 // toggles the EQ window
#define WINAMP_OPTIONS_PLEDIT           40040 // toggles the playlist window
#define WINAMP_VOLUMEUP                 40058 // Raise volume by 1%
#define WINAMP_VOLUMEDOWN               40059 // Lower volume by 1%
#define WINAMP_FFWD5S                   40060 // fast forwards 5 seconds
#define WINAMP_REW5S                    40061 // rewinds 5 seconds

#define WINAMP_BUTTON1                  40044 // Previous track button 40044
#define WINAMP_BUTTON2                  40045 // Play button 40045
#define WINAMP_BUTTON3                  40046 // Pause/Unpause button 40046
#define WINAMP_BUTTON4                  40047 // Stop button 40047
#define WINAMP_BUTTON5                  40048 // Next track button 40048
#define WINAMP_BUTTON1_SHIFT            40144 // Fast-rewind 5 seconds 40144
#define WINAMP_BUTTON2_SHIFT            40145
#define WINAMP_BUTTON3_SHIFT            40146
#define WINAMP_BUTTON4_SHIFT            40147 // Fadeout and stop 40147
#define WINAMP_BUTTON5_SHIFT            40148 // Fast-forward 5 seconds 40148
#define WINAMP_BUTTON1_CTRL             40154 // Start of playlist 40154
#define WINAMP_BUTTON2_CTRL             40155 // Open URL dialog 40155
#define WINAMP_BUTTON3_CTRL             40156
#define WINAMP_BUTTON4_CTRL             40157 // Stop after current track 40157
#define WINAMP_BUTTON5_CTRL             40158 // Go to end of playlist  40158

#define WINAMP_FILE_PLAY                40029 // pops up the load file(s) box
#define WINAMP_OPTIONS_PREFS            40012 // pops up the preferences
#define WINAMP_HELP_ABOUT               40041 // pops up the about box
#define WINAMP_FILE_INFO                40188 // Open file info box 40188
#define WINAMP_DISPLAY_ELAPSED          40037 // Set time display mode to elapsed 40037
#define WINAMP_DISPLAY_REMAINING        40038 // Set time display mode to remaining 40038
#define WINAMP_O_VISUAL_OPTIONS         40190 // Open visualization options 40190
#define WINAMP_O_VISUAL_PLUGIN_OPTIONS  40191 // Open visualization plug-in options 40191
#define WINAMP_EXECUTE_VISUAL_PLUGIN    40192 // Execute current visualization plug-in 40192
#define WINAMP_T_TITLE_SCROLLING        40189 // Toggle title Autoscrolling 40189
#define WINAMP_T_ALWAYS_ON_TOP          40019 // Toggle always on top 40019
#define WINAMP_T_WINDOWSHADE            40064 // toggle Windowshade 40064
#define WINAMP_T_PLIST_WINDOWSHADE      40266 // Toggle Playlist Windowshade 40266
#define WINAMP_T_DOUBLESIZE             40165 // Toggle doublesize mode 40165
#define WINAMP_T_MAIN_WINDOW            40258 // Toggle main window visible 40258
#define WINAMP_T_MINIBROWSER            40298 // Toggle minibrowser 40298
#define WINAMP_T_EASYMOVE               40186 // Toggle easymove 40186
#define WINAMP_T_REPEAT                 40022 // Toggle repeat 40022
#define WINAMP_T_SHUFFLE                40023 // Toggle shuffle 40023
#define WINAMP_O_JUMP_TO_TIME           40193 // Open jump to time dialog 40193
#define WINAMP_O_JUMP_TO_FILE           40194 // Open jump to file dialog 40194
#define WINAMP_O_SKIN_SELECTOR          40219 // Open skin selector 40219
#define WINAMP_CONFIG_VISUAL_PLUGIN     40221 // Configure current visualization plug-in 40221
#define WINAMP_RELOAD_CURRENT_SKIN      40291 // Reload the current skin 40291
#define WINAMP_CLOSE_WINAMP             40001 // Close Winamp 40001
#define WINAMP_10_TRACKS_BACK           40197 // Moves back 10 tracks in playlist 40197
#define WINAMP_SHOW_EDIT_BOOKMARKS      40320 // Show the edit bookmarks 40320
#define WINAMP_ADD_CUR_TRACK_BOOKMARK   40321 // Adds current track as a bookmark 40321
#define WINAMP_PLAY_AUDIO_CD            40323 // Play audio CD 40323

#define WINAMP_LOAD_PRESET_FROM_EQ      40243 // Load a preset from EQ 40253
#define WINAMP_SAVE_PRESET_TO_EQF       40254 // Save a preset to EQF 40254
#define WINAMP_O_LOAD_PRESET            40172 // Opens load presets dialog 40172
#define WINAMP_O_LOAD_AUTOPRESET        40173 // Opens auto-load presets dialog 40173
#define WINAMP_LOAD_DEFAULT_PRESET      40174 // Load default preset 40174
#define WINAMP_O_SAVE_PRESET            40175 // Opens save preset dialog 40175
#define WINAMP_O_AUTOLOAD_SAVE_PRESET   40176 // Opens auto-load save preset 40176
#define WINAMP_O_DELETE_PRESET          40178 // Opens delete preset dialog 40178
#define WINAMP_O_DELETE_AUTOPRESET      40180 // Opens delete an auto load preset dialog 40180

typedef struct tagPProServices
{
	void (*ErrMessage)(LPSTR, LPSTR);
	BOOL (*MatchCaption)(HWND, LPSTR);
	HWND (*FindMatchingWindow)(LPSTR,BOOL);
	BOOL (*IsRolled)(HWND hw);
	BOOL (*IsTrayMinned)(HWND hw);
	void (*GetExeFullPath)(HWND hw, LPSTR szt);
	void (*RollUp)(HWND hw);
	void (*TrayMin)(HWND hw);
	void (*SendKeys)(LPSTR sz);
} PPROSERVICES;

static HWND g_hwndPowerPro;

#pragma warn -sus

// Main DLL entry point.  Save handle to main PowerPro window.
#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    if(dwReason == DLL_PROCESS_ATTACH)
    {
		g_hwndPowerPro = FindWindow("PowerProMain",NULL);
    }
	else if(dwReason == DLL_PROCESS_DETACH)
    {
		// Detaching process
    }
    return TRUE;
}

static void MakeAction(UINT sw, UINT nargs, LPSTR * szargs, DWORD * pFlags, PPROSERVICES * ppsv)
  {
   if (nargs!=2 && !(sw==IPC_JUMPTOTIME||sw==IPC_SETPLAYLISTPOS||sw==SET_VOLUME||sw==IPC_SETVOLUME||sw==IPC_SETPANNING||sw==SET_PANNING
                     ||sw==IPC_CHDIR||sw==IPC_PLAYFILE||sw==IPC_GETEQDATA||sw==IPC_SETEQDATA||sw==SET_EQ_DATA||sw==GET_EQ_DATA))
     {
      (ppsv->ErrMessage)("Need two arguments for the service.","");
     }
   else
      if (nargs != 3 && (sw==IPC_JUMPTOTIME||sw==IPC_SETPLAYLISTPOS||sw==SET_VOLUME||sw==IPC_SETVOLUME||sw==IPC_SETPANNING||sw==SET_PANNING
                         ||sw==IPC_CHDIR||sw==IPC_PLAYFILE||sw==IPC_GETEQDATA||sw==GET_EQ_DATA))
        {
         (ppsv->ErrMessage)("Need three arguments for the service.","");
        }
      else
         if (nargs != 4 && (sw==IPC_SETEQDATA||sw==SET_EQ_DATA))
           {
            (ppsv->ErrMessage)("Need four arguments for the service.","");
           }
         else
	   {
            HWND hwndWinamp = NULL;

            if (strcmp ((*(szargs+1)),"") == 0)
              {
               hwndWinamp = FindWindow("Winamp v1.x",NULL);
              }
            else
              {
               hwndWinamp = FindWindow((*(szargs+1)),NULL);
              }

            if (hwndWinamp)
              {
               if (strcmp ((*(szargs+2)),"2") == 0)    //if second argument is 2, set flag f0 to 1
                 {
                  (*pFlags) = (*pFlags)|0x00000001;
                 }
               switch (sw)
                 {
                  case SET_PANNING:
                     SendMessage(hwndWinamp, WM_USER, atoi(*(szargs+3))*(1.27), IPC_SETPANNING);
                     **szargs = '\0';
                     break;                                 
                  case GET_POSITION_IN_SECONDS:
                    {
                     double ret = 0;
                     ret = SendMessage(hwndWinamp, WM_USER, 0, IPC_GETOUTPUTTIME);
                     if (ret != -1)
                       {
                        ret /=1000;
                        sprintf (*szargs,"%.0f",ret);
                       }
                     else
                       {
                        sprintf (*szargs,"%.0f",ret);
                       }
                    }
                    break;
                  case GET_EQ_DATA:
                    {
                     char * p;
                     double ret = 0;
                     int param = 0;
                     param = atoi(*(szargs+3));
                     ret = SendMessage(hwndWinamp, WM_USER, param, IPC_GETEQDATA);
                     if ((param >= 0) && (param <= 10))
                       {
                        ret = ret * 40 / 63;
                        ret -= 20;
                        ret = -1 * ret;
                        sprintf (*szargs, "%.1f", ret);
                       }
                     else
                       {
                        itoa (ret, *szargs, 10);
                       }
                    }
                    break;
               case SET_EQ_DATA:
                  {
                   int param1 = 0;
                   double param2 = 0;
                   param1 = atoi(*(szargs+3));
                   if ((param1 >= 0) && (param1 <= 10))
                      {
                       param2 = atof(*(szargs+4));
                       param2 = (-1) * param2;
                       param2 += 20;
                       param2 = param2 * 63 / 40;
                      }
                   else
                      {
                       param2 = atoi(*(szargs+4));
                      }
                   SendMessage(hwndWinamp,WM_USER,param1,IPC_GETEQDATA);
                   SendMessage(hwndWinamp,WM_USER,param2,IPC_SETEQDATA);
                   **szargs = '\0';
                  }
                  break;
               case IPC_SETEQDATA:
                  SendMessage(hwndWinamp,WM_USER,atoi(*(szargs+3)),IPC_GETEQDATA);
                  SendMessage(hwndWinamp,WM_USER,atoi(*(szargs+4)),IPC_SETEQDATA);
                  break;
               case IPC_GETEQDATA:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, atoi(*(szargs+3)), IPC_GETEQDATA);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case IPC_PLAYFILE:
                  {
                   COPYDATASTRUCT cds;
                   cds.dwData = IPC_PLAYFILE;
                   cds.lpData = (void *) (*(szargs+3));
                   cds.cbData = strlen((char *) cds.lpData)+1;
                   SendMessage(hwndWinamp,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
                  }
                  break;
               case IPC_CHDIR:
                  {
                   COPYDATASTRUCT cds;
                   cds.dwData = IPC_CHDIR;
                   cds.lpData = (void *) (*(szargs+3));
                   cds.cbData = strlen((char *) cds.lpData)+1;
                   SendMessage(hwndWinamp,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
                  }
                  break;
               case BLOCK_MINIBROWSER:
                  SendMessage(hwndWinamp, WM_USER, 1, IPC_MBBLOCK);
                  **szargs = '\0';
                  break;
               case UNBLOCK_MINIBROWSER:
                  SendMessage(hwndWinamp, WM_USER, 0, IPC_MBBLOCK);
                  **szargs = '\0';
                  break;
               case SHUFFLE_ON:
                  SendMessage(hwndWinamp, WM_USER, 1, IPC_SET_SHUFFLE);
                  **szargs = '\0';
                  break;
               case SHUFFLE_OFF:
                  SendMessage(hwndWinamp, WM_USER, 0, IPC_SET_SHUFFLE);
                  **szargs = '\0';
                  break;
               case REPEAT_ON:
                  SendMessage(hwndWinamp, WM_USER, 1, IPC_SET_REPEAT);
                  **szargs = '\0';
                  break;
               case REPEAT_OFF:
                  SendMessage(hwndWinamp, WM_USER, 0, IPC_SET_REPEAT);
                  **szargs = '\0';
                  break;
               case IPC_GET_REPEAT:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 0, IPC_GET_REPEAT);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case IPC_GET_SHUFFLE:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 0, IPC_GET_SHUFFLE);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case IPC_REFRESHPLCACHE:
                  SendMessage(hwndWinamp, WM_USER, 0, IPC_REFRESHPLCACHE);
                  **szargs = '\0';
                  break;
               case IPC_UPDTITLE:
                  SendMessage(hwndWinamp, WM_USER, 0, IPC_UPDTITLE);
                  **szargs = '\0';
                  break;
               case IPC_INETAVAILABLE:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 0, IPC_INETAVAILABLE);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case IPC_RESTART_WINAMP:
                  SendMessage(hwndWinamp, WM_USER, 0, IPC_RESTART_WINAMP);
                  **szargs = '\0';
                  break;
               case GET_NUMBER_OF_CHANNELS:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 2, IPC_GETINFO);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case GET_BITRATE:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 1, IPC_GETINFO);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case GET_SAMPLERATE:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 0, IPC_GETINFO);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case IPC_GETPOSITION:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 1, IPC_GETPOSITION);
                   itoa (ret+1, *szargs, 10);
                  }
                  break;
               case IPC_WRITEPLAYLIST:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 1, IPC_WRITEPLAYLIST);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case IPC_GETLISTLENGTH:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 1, IPC_GETLISTLENGTH);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case IPC_SETPANNING:
                  SendMessage(hwndWinamp, WM_USER, atoi(*(szargs+3)), IPC_SETPANNING);
                  **szargs = '\0';
                  break;
               case SET_VOLUME:
                  SendMessage(hwndWinamp, WM_USER, atoi(*(szargs+3))*255/100, IPC_SETVOLUME);
                  **szargs = '\0';
                  break;
               case IPC_SETVOLUME:
                  SendMessage(hwndWinamp, WM_USER, atoi(*(szargs+3)), IPC_SETVOLUME);
                  **szargs = '\0';
                  break;
               case IPC_SETPLAYLISTPOS:
                  SendMessage(hwndWinamp, WM_USER, atoi(*(szargs+3)), IPC_SETPLAYLISTPOS);
                  **szargs = '\0';
                  break;
               case IPC_JUMPTOTIME:
                  SendMessage(hwndWinamp, WM_USER, atoi(*(szargs+3)), IPC_JUMPTOTIME);
                  **szargs = '\0';
                  break;
               case GET_POSITION:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 0, IPC_GETOUTPUTTIME);
                   itoa (ret, *szargs, 10);
                  }
                  break;
                case GET_LENGTH:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 1, IPC_GETOUTPUTTIME);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case IPC_ISPLAYING:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 0, IPC_ISPLAYING);
                   itoa (ret, *szargs, 10);
                  }
                  break;
               case IPC_STARTPLAY:
                  SendMessage(hwndWinamp, WM_USER, 0, IPC_STARTPLAY);
                  **szargs = '\0';
                  break;
               case IPC_DELETE:
                  SendMessage(hwndWinamp, WM_USER, 0, IPC_DELETE);
                  **szargs = '\0';
                  break;
               case IPC_GETVERSION:
                  {
                   int ret = 0;
                   ret = SendMessage(hwndWinamp, WM_USER, 0, IPC_GETVERSION);
                   itoa (ret, *szargs, 16);
                  }
                  break;
               case WINAMP_O_DELETE_AUTOPRESET:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_DELETE_AUTOPRESET, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_DELETE_PRESET:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_DELETE_PRESET, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_AUTOLOAD_SAVE_PRESET:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_AUTOLOAD_SAVE_PRESET, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_SAVE_PRESET:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_SAVE_PRESET, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_LOAD_DEFAULT_PRESET:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_LOAD_DEFAULT_PRESET, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_LOAD_AUTOPRESET:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_LOAD_AUTOPRESET, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_LOAD_PRESET:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_LOAD_PRESET, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_SAVE_PRESET_TO_EQF:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_SAVE_PRESET_TO_EQF, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_LOAD_PRESET_FROM_EQ:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_LOAD_PRESET_FROM_EQ, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_PLAY_AUDIO_CD:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_PLAY_AUDIO_CD, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_ADD_CUR_TRACK_BOOKMARK:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_ADD_CUR_TRACK_BOOKMARK, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_SHOW_EDIT_BOOKMARKS:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_SHOW_EDIT_BOOKMARKS, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_10_TRACKS_BACK:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_10_TRACKS_BACK, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_CLOSE_WINAMP:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_CLOSE_WINAMP, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_RELOAD_CURRENT_SKIN:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_RELOAD_CURRENT_SKIN, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_CONFIG_VISUAL_PLUGIN:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_CONFIG_VISUAL_PLUGIN, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_SKIN_SELECTOR:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_SKIN_SELECTOR, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_JUMP_TO_FILE:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_JUMP_TO_FILE, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_JUMP_TO_TIME:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_JUMP_TO_TIME, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_SHUFFLE:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_SHUFFLE, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_REPEAT:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_REPEAT, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_EASYMOVE:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_EASYMOVE, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_MINIBROWSER:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_MINIBROWSER, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_MAIN_WINDOW:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_MAIN_WINDOW, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_DOUBLESIZE:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_DOUBLESIZE, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_PLIST_WINDOWSHADE:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_PLIST_WINDOWSHADE, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_WINDOWSHADE:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_WINDOWSHADE, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_TITLE_SCROLLING:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_TITLE_SCROLLING, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_EXECUTE_VISUAL_PLUGIN:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_EXECUTE_VISUAL_PLUGIN, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_VISUAL_PLUGIN_OPTIONS:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_VISUAL_PLUGIN_OPTIONS, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_O_VISUAL_OPTIONS:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_O_VISUAL_OPTIONS, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_DISPLAY_REMAINING:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_DISPLAY_REMAINING, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_DISPLAY_ELAPSED:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_DISPLAY_ELAPSED, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_FILE_INFO:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_FILE_INFO, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_HELP_ABOUT:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_HELP_ABOUT, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_T_ALWAYS_ON_TOP:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_T_ALWAYS_ON_TOP, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_OPTIONS_PREFS:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_OPTIONS_PREFS, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_FILE_PLAY:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_FILE_PLAY, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_BUTTON5_CTRL:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON5_CTRL, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_BUTTON4_CTRL:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON4_CTRL, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_BUTTON2_CTRL:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON2_CTRL, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_BUTTON1_CTRL:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON1_CTRL, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_BUTTON4_SHIFT:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON4_SHIFT, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_BUTTON1:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON1, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_BUTTON2:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON2, 0);
                  **szargs = '\0';
				      break;
               case WINAMP_BUTTON3:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON3, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_BUTTON4:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON4, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_BUTTON5:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_BUTTON5, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_FFWD5S:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_FFWD5S, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_REW5S:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_REW5S, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_OPTIONS_EQ:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_OPTIONS_EQ, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_OPTIONS_PLEDIT:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_OPTIONS_PLEDIT, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_VOLUMEDOWN:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_VOLUMEDOWN, 0);
                  **szargs = '\0';
                  break;
               case WINAMP_VOLUMEUP:
                  PostMessage (hwndWinamp, WM_COMMAND, WINAMP_VOLUMEUP, 0);
                  **szargs = '\0';
                  break;
               case GET_SONG_NAME:
                  {
                   char this_title[280],*p;

                   GetWindowText(hwndWinamp,this_title,sizeof(this_title));

                   p = this_title+strlen(this_title)-8;
                   while (p >= this_title)
                      {
                       if (!strnicmp(p,"- Winamp",8)) break;
                       p--;
                      }
                   if (p >= this_title) p--;
                   while (p >= this_title && *p == ' ') p--;
                   *++p=0;
                   this_title[263] = '\0';
                   p = this_title;
                   while (*p != ' ')
                      {
                       p++;
                      }
                   p++;
                   strcpy (*szargs, p);
                  }
                  break;
               case GET_SONG_NAME_AND_NUMBER:
                  {
                   char this_title[280],*p;

                   GetWindowText(hwndWinamp,this_title,sizeof(this_title));

                   p = this_title+strlen(this_title)-8;
                   while (p >= this_title)
                      {
                       if (!strnicmp(p,"- Winamp",8)) break;
                       p--;
                      }
                   if (p >= this_title) p--;
                   while (p >= this_title && *p == ' ') p--;
                   *++p=0;
                   this_title[263] = '\0';
                   strcpy (*szargs, this_title);
                  }
                  break;
               case GET_CAPTION:
                  {
                   char this_title[280];

                   GetWindowText (hwndWinamp, this_title, sizeof (this_title));

                   this_title[263] = '\0';
                   strcpy (*szargs, this_title);
                  }
                  break;
		        }
          }
       else
          {
           if (strcmp ((*(szargs+2)),"1") == 0)    //if second argument is 1 then show window with error explanation
              {
               (ppsv->ErrMessage)("Specified Winamp window does not exist.","");
              }
           else      // else, if second argument is 2 then set flag f0 to 0
              {
               if (strcmp ((*(szargs+2)),"2") == 0)
                  {
                   (*pFlags) = (*pFlags)&0xFFFFFFFE;
                  }
              }
          }
      }
}

_declspec(dllexport) void song_name(LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_SONG_NAME, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void caption(LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_CAPTION, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void song_and_number(LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_SONG_NAME_AND_NUMBER, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_eq_window (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_OPTIONS_EQ, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_plist_window (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_OPTIONS_PLEDIT, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void volume_up (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_VOLUMEUP, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void volume_down (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_VOLUMEDOWN, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void forward_5sec (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_FFWD5S, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void rewind_5sec (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_REW5S, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void previous_track (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON1, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void play_button (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON2, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void pause_unpause (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON3, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void stop (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON4, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void next_track (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON5, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void stop_with_fadeout (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON4_SHIFT, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void start_of_plist (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON1_CTRL, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void open_url_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON2_CTRL, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void stop_after_current (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON4_CTRL, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void end_of_plist (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_BUTTON5_CTRL, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void file_open_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_FILE_PLAY, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void open_preferences (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_OPTIONS_PREFS, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_on_top (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_ALWAYS_ON_TOP, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void open_about_box (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_HELP_ABOUT, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void open_file_info_box (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_FILE_INFO, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void display_elapsed_time (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_DISPLAY_ELAPSED, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void display_remaining_time (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_DISPLAY_REMAINING, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void open_visual_options (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_VISUAL_OPTIONS, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void open_visual_plugin_options (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_VISUAL_PLUGIN_OPTIONS, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void execute_visual_plugin (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_EXECUTE_VISUAL_PLUGIN, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_title_scrolling (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_TITLE_SCROLLING, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_windowshade (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_WINDOWSHADE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_plist_windowshade (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_PLIST_WINDOWSHADE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_doublesize (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_DOUBLESIZE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_main_window (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_MAIN_WINDOW, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_minibrowser (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_MINIBROWSER, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_easymove (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_EASYMOVE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_repeat (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_REPEAT, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void toggle_shuffle (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_T_SHUFFLE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void jump_to_time_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_JUMP_TO_TIME, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void jump_to_file_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_JUMP_TO_FILE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void open_skin_selector (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_SKIN_SELECTOR, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void configure_visual_plugin (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_CONFIG_VISUAL_PLUGIN, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void reload_current_skin (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_RELOAD_CURRENT_SKIN, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void close_winamp (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_CLOSE_WINAMP, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void move_10_tracks_back (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_10_TRACKS_BACK, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void show_edit_bookmarks (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_SHOW_EDIT_BOOKMARKS, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void add_track_as_bookmark (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_ADD_CUR_TRACK_BOOKMARK, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void play_audio_cd (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_PLAY_AUDIO_CD, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void load_preset_from_eq (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_LOAD_PRESET_FROM_EQ, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void save_preset_to_eqf (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_SAVE_PRESET_TO_EQF, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void load_preset_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_LOAD_PRESET, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void autoload_preset_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_LOAD_AUTOPRESET, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void load_default_preset (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_LOAD_DEFAULT_PRESET, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void save_preset_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_SAVE_PRESET, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void autoload_save_preset_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_AUTOLOAD_SAVE_PRESET, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void delete_preset_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_DELETE_PRESET, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void delete_autoload_preset_dialog (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (WINAMP_O_DELETE_AUTOPRESET, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_version (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_GETVERSION, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void delete_plist (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_DELETE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void play_selected (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_STARTPLAY, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_playback_status (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_ISPLAYING, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_position (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_POSITION, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_length (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_LENGTH, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void jump_to_time (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_JUMPTOTIME, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void set_plist_position (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_SETPLAYLISTPOS, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void set_volume (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (SET_VOLUME, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void set_volume255 (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_SETVOLUME, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void set_panning127 (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_SETPANNING, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_plist_length (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_GETLISTLENGTH, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_plist_position (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_GETPOSITION, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_plist_position1 (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_WRITEPLAYLIST, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_samplerate (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_SAMPLERATE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_bitrate (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_BITRATE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_number_of_channels (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_NUMBER_OF_CHANNELS, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void restart_winamp (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_RESTART_WINAMP, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_net_status (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_INETAVAILABLE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void update_info (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_UPDTITLE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void flush_plist_cache_buffer (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_REFRESHPLCACHE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_shuffle (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_GET_SHUFFLE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_repeat (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_GET_REPEAT, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void repeat_on (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (REPEAT_ON, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void repeat_off (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (REPEAT_OFF, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void shuffle_on (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (SHUFFLE_ON, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void shuffle_off (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (SHUFFLE_OFF, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void block_minibrowser (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (BLOCK_MINIBROWSER, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void unblock_minibrowser (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (UNBLOCK_MINIBROWSER, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void change_directory (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_CHDIR, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void add_file_to_plist (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_PLAYFILE, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_eq_data63 (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_GETEQDATA, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void set_eq_data63 (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (IPC_SETEQDATA, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_eq_data (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_EQ_DATA, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void set_eq_data (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (SET_EQ_DATA, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void get_position_in_sec (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (GET_POSITION_IN_SECONDS, nargs, szargs, pFlags, ppsv);
}

_declspec(dllexport) void set_panning (LPSTR szv, LPSTR szx, BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD * pFlags, UINT nargs, LPSTR * szargs, PPROSERVICES * ppsv)
{
   MakeAction (SET_PANNING, nargs, szargs, pFlags, ppsv);
}
