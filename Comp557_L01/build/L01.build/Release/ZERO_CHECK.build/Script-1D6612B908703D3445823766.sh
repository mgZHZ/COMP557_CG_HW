#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/hanzhizhang/Desktop/MG/Fall22/COMP557/Comp557_L01/build
  make -f /Users/hanzhizhang/Desktop/MG/Fall22/COMP557/Comp557_L01/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/hanzhizhang/Desktop/MG/Fall22/COMP557/Comp557_L01/build
  make -f /Users/hanzhizhang/Desktop/MG/Fall22/COMP557/Comp557_L01/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/hanzhizhang/Desktop/MG/Fall22/COMP557/Comp557_L01/build
  make -f /Users/hanzhizhang/Desktop/MG/Fall22/COMP557/Comp557_L01/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/hanzhizhang/Desktop/MG/Fall22/COMP557/Comp557_L01/build
  make -f /Users/hanzhizhang/Desktop/MG/Fall22/COMP557/Comp557_L01/build/CMakeScripts/ReRunCMake.make
fi

