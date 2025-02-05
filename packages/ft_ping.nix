{stdenv}:
stdenv.mkDerivation {
  src = ./..;
  name = "ft_ping";
  installPhase = ''
    mkdir -p $out/bin
    cp ft_ping $out/bin
  '';
}
