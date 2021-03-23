/* vim: set expandtab ts=2 sw=2: */
/* options.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "scrot.h"
#include "options.h"

static void scrot_parse_option_array(int argc, char **argv);
scrotoptions opt;

void
init_parse_options(int argc, char **argv)
{
  /* Set default options */
  memset(&opt, 0, sizeof(scrotoptions));

  opt.quality = 75;
  opt.border = 1;

  /* Parse the cmdline args */
  scrot_parse_option_array(argc, argv);
}

static void
scrot_parse_option_array(int argc, char **argv)
{
   static char stropts[] = "abcd:e:hmnqw:st:uv+:z";
   static struct option lopts[] = {
      /* actions */
      {"help", 0, 0, 'h'},                  /* okay */
      {"version", 0, 0, 'v'},               /* okay */
      {"count", 0, 0, 'c'},
      {"select", 0, 0, 's'},
      {"focused", 0, 0, 'u'},
      {"focussed", 0, 0, 'u'},	/* macquarie dictionary has both spellings */
      {"border", 0, 0, 'b'},
      {"no-border", 0, 0, 'n'},
      {"no-decorations", 0, 0, 'n'},
      {"alpha", 0, 0, 'a'},
      {"multidisp", 0, 0, 'm'},
      {"silent", 0, 0, 'z'},
      /* toggles */
      {"window", 1, 0, 'w'},
      {"thumb", 1, 0, 't'},
      {"delay", 1, 0, 'd'},
      {"quality", 1, 0, 'q'},
      {"exec", 1, 0, 'e'},
      {"debug-level", 1, 0, '+'},
      {0, 0, 0, 0}
   };
   int optch = 0, cmdx = 0;

   /* Now to pass some optionarinos */
   while ((optch = getopt_long(argc, argv, stropts, lopts, &cmdx)) !=
          EOF)
   {
      switch (optch)
      {
        case 0:
           break;
        case 'h':
           show_usage();
           break;
        case 'v':
           show_version();
           break;
        case 'a':
           opt.alpha = 1;
           /* fallthrough */
        case 'b':
           opt.border = 1;
           break;
        case 'n':
           opt.alpha = 0;
           opt.border = 0;
		       break;
        case 'd':
           opt.delay = atoi(optarg);
           break;
        case 'e':
           opt.exec = gib_estrdup(optarg);
           break;
        case 'm':
           opt.multidisp = 1;
           break;
        case 'q':
           opt.quality = atoi(optarg);
           break;
        case 's':
           opt.select = 1;
           break;
        case 'u':
           opt.focused = 1;
           break;
        case 'w':
           opt.window = strtol(optarg, NULL, 16);
           break;
        case '+':
           opt.debug_level = atoi(optarg);
           break;
        case 'c':
           opt.countdown = 1;
           break;
        case 't':
           options_parse_thumbnail(optarg);
           break;
        case 'z':
           opt.silent = 1;
           break;
        default:
           break;
      }
   }

   /* Now the leftovers, which must be files */
   if (optind < argc)
   {
      while (optind < argc)
      {
         /* If recursive is NOT set, but the only argument is a directory
            name, we grab all the files in there, but not subdirs */
         if (!opt.output_file)
         {
            opt.output_file = argv[optind++];

            if ( strlen(opt.output_file) > 256 ) {
               printf("output filename too long.\n");
               exit(EXIT_FAILURE);
            }

            if (opt.thumb)
               opt.thumb_file = name_thumbnail(opt.output_file);
         }
         else
            gib_weprintf("unrecognised option %s\n", argv[optind++]);
      }
   }

   /* So that we can safely be called again */
   optind = 1;
}

char *
name_thumbnail(char *name)
{
   size_t length = 0;
   char *new_title;
   char *dot_pos;
   size_t diff = 0;

   length = strlen(name) + 7;
   new_title = gib_emalloc(length);

   dot_pos = strrchr(name, '.');
   if (dot_pos)
   {
      diff = (dot_pos - name) / sizeof(char);

      strncpy(new_title, name, diff);
      strcat(new_title, "-thumb");
      strcat(new_title, dot_pos);
   }
   else
      sprintf(new_title, "%s-thumb", name);

   return new_title;
}

void
options_parse_thumbnail(char *optarg)
{
   char *tok;

   if (strchr(optarg, 'x')) /* We want to specify the geometry */
   {
     tok = strtok(optarg, "x");
     opt.thumb_width = atoi(tok);
     tok = strtok(NULL, "x");
     if (tok)
     {
       opt.thumb_width = atoi(optarg);
       opt.thumb_height = atoi(tok);

       if (opt.thumb_width < 0)
         opt.thumb_width = 1;
       if (opt.thumb_height < 0)
         opt.thumb_height = 1;

       if (!opt.thumb_width && !opt.thumb_height)
         opt.thumb = 0;
       else
         opt.thumb = 1;
     }
   }
   else
   {
     opt.thumb = atoi(optarg);
     if (opt.thumb < 1)
       opt.thumb = 1;
     else if (opt.thumb > 100)
       opt.thumb = 100;
   }
}

void
show_version(void)
{
   printf(PACKAGE " version " VERSION "\n");
   exit(0);
}

void
show_mini_usage(void)
{
   printf("Usage : " PACKAGE " [OPTIONS]... FILE\nUse " PACKAGE
          " --help for detailed usage information\n");
   exit(0);
}


void
show_usage(void)
{
   fprintf(stdout,
           "Usage : " PACKAGE " [OPTIONS]... [FILE]\n"
           "  Where FILE is the target file for the screenshot.\n"
           "  If FILE is not specified, a date-stamped file will be dropped in the\n"
           "  current directory.\n" "  See man " PACKAGE " for more details\n"
           "  -h, --help                Display this help and exit.\n"
           "  -v, --version             Output version information and exit.\n"
           "  -c, --count               Show a countdown before taking the shot.\n"
           "  -d, --delay NUM           Wait NUM seconds before taking a shot.\n"
           "  -e, --exec APP            Run APP on the resulting screenshot.\n"
           "  -q, --quality NUM         Image quality (1-100) high value means\n"
           "                            high size, low compression. Default: 75.\n"
           "                            For lossless compression formats, like png,\n"
           "                            low quality means high compression.\n"
           "  -m, --multidisp           For multiple heads, grab shot from each\n"
           "                            and join them together.\n"
           "  -s, --select              Interactively choose a window or rectangle\n"
           "                            with the mouse.\n"
           "  -a, --alpha               When selecting a window, create semi-transparent\n"
           "                            screenshot that includes window shadows.\n"
           "  -n, --no-decorations      When selecting a window, don't grab\n"
           "                            window decorations (border, titlebar, etc).\n"
           "  -u, --focused             Use the currently focused window.\n"
           "  -w, --window ID           Use the window identified by ID.\n"
           "  -t, --thumb NUM           Generate thumbnail too. NUM is the percentage\n"
           "                            of the original size for the thumbnail to be,\n"
           "                            or the geometry in percent, e.g. 50x60 or 80x20.\n"
           "  -z, --silent              Prevent beeping\n"
           "\n" "  SPECIAL STRINGS\n"
           "  Both the --exec and filename parameters can take format specifiers\n"
           "  that are expanded by " PACKAGE " when encountered.\n"
           "  There are two types of format specifier. Characters preceded by a '%%'\n"
           "  are interpreted by strftime(2). See man strftime for examples.\n"
           "  These options may be used to refer to the current date and time.\n"
           "  The second kind are internal to " PACKAGE
           "  and are prefixed by '$'\n"
           "  The following specifiers are recognised:\n"
           "                  $f image path/filename (ignored when used in the filename)\n"
           "                  $m thumbnail path/filename\n"
           "                  $n image name (ignored when used in the filename)\n"
           "                  $s image size (bytes) (ignored when used in the filename)\n"
           "                  $p image pixel size\n"
           "                  $w image width\n"
           "                  $h image height\n"
           "                  $t image format\n"
           "                  $$  prints a literal '$'\n"
           "                  \\n prints a newline (ignored when used in the filename)\n"
           "  Example:\n" "          " PACKAGE
           " '%%Y-%%m-%%d_$wx$h_scrot.png' -e 'mv $f ~/images/shots/'\n"
           "          Creates a file called something like 2000-10-30_2560x1024_scrot.png\n"
           "          and moves it to your images directory.\n" "\n"
           "This program is free software see the file COPYING for licensing info.\n"
           "Copyright Tom Gilbert 2000\n"
           "Email bugs to <scrot_sucks@linuxbrit.co.uk>\n");
   exit(0);
}

