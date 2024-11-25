FROM devkitpro/devkitppc:20241116

COPY --from=wiiuenv/libgui:20220205 /artifacts $DEVKITPRO

WORKDIR project
