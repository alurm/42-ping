{
  inetutils,
  fetchurl,
}:
inetutils.overrideAttrs (old: rec {
  version = "2.0";
  src = fetchurl {
    url = "mirror://gnu/${old.pname}/${old.pname}-${version}.tar.xz";
    hash = "sha256-5XPVZuVTk5QAmYYuf4mUFkoO0S9ahsM0U4CEK9wSRyI=";
  };
})
