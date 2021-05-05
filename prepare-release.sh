#!/bin/sh

set -e

err() {
	for e in "$@"; do
		echo "$e" >&2
	done
	exit 1
}

usage() {
	err "$@" "usage: ${0##*/} tag [savepath]"
}

test $# -gt 0 || usage
test $# -le 2 || usage

tag=$1; shift
echo "$tag" | egrep -q '^v[0-9]+\.[0-9]+\.[0-9]+$' ||
	err "invalid tag name, must be like v1.2.3"

arc_path=${1:-/tmp}

v=${tag#v}
arc_name=tlc-${v}.tar.gz

test -f CHANGES || err "${0##*/} must be called from source root"
! grep -Fqx "HEAD" CHANGES || err "CHANGES must not contain HEAD section"
  grep -Fqx "v.$v" CHANGES || err "CHANGES lacks $v section"

git branch | grep -Fqx '* master' || err "not on master branch"

if git tag | fgrep -qx "$tag"; then
	echo "tag $tag is already there, skipping it's creation"
else
	if git status -s CHANGES | grep 'M CHANGES'; then
		git ci CHANGES -m "version $v"
		git push origin
	fi
	git tag "$tag"
	git push origin "$tag"
fi

pax -wzv \
	-s ',^\./.git.*,,' \
	-s ',^\./obj\(/.*\)*$,,' \
	-s ',^\./build.*,,' \
	-s ",^./${0##*/}\$,," \
	-s "/^./tlc-${v}/" \
	. >"$arc_path/$arc_name"
echo "archive saved to $arc_path/$arc_name"
