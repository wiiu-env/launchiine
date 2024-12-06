FROM devkitpro/devkitppc:latest

COPY --from=wiiuenv/libgui:20220205 /artifacts $DEVKITPRO

WORKDIR project
