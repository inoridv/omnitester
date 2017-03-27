#!/bin/bash
set -e

PATHS="
drupal_module/phantomsheet
:!*.less
:!*.png
:!*.json
:!*.csv
"

CHANGED_FILES="$(git diff --name-only --diff-filter=d origin/stage -- $PATHS)"

if [[ -n "${CHANGED_FILES// }" ]]; then
  echo "$CHANGED_FILES" | xargs phpcs -v --standard=Drupal,DrupalPractice

  for FILE in $CHANGED_FILES; do
    php -l $FILE
  done
fi
