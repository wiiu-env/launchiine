FROM wiiuenv/devkitppc:20200625

COPY --from=wiiuenv/libgui:20200626 /artifacts $DEVKITPRO

WORKDIR project