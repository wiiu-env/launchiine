FROM wiiuenv/devkitppc:20211229

COPY --from=wiiuenv/libgui:20220109 /artifacts $DEVKITPRO

WORKDIR project