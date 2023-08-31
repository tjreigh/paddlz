# ----------------------------
# Set NAME to the program name
# Set ICON to the png icon file name
# Set DESCRIPTION to display within a compatible shell
# Set COMPRESSED to "YES" to create a compressed program
# ----------------------------

NAME        ?= PONG
COMPRESSED  ?= NO
ICON        ?= icon.png
DESCRIPTION ?= "Pong game made by Trevor Reigh"

# ----------------------------

include $(CEDEV)/meta/makefile.mk
