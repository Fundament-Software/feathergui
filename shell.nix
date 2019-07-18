{ pkgs ? import <nixpkgs> {} }:

with pkgs;

mkShell {
  buildInputs = [
    glibc
    xorg.libX11
  ];
  INCLUDE_PATH="./;${xorg.libX11.dev}/include;${glibc.dev}/include;${xorg.xorgproto}/include";
  LIBRARY_PATH="${xorg.libX11}/lib";
  #LD_LIBRARY_PATH="${xorg.libX11}/lib:./";
}
