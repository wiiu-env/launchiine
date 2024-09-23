FROM ghcr.io/wiiu-env/devkitppc:20240704

COPY --from=wiiuenv/libgui:2022020513505562e265 /artifacts $DEVKITPRO

WORKDIR project
