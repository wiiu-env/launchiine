FROM devkitpro/devkitppc:20250102

COPY --from=wiiuenv/libgui:20220205 /artifacts $DEVKITPRO

WORKDIR project
