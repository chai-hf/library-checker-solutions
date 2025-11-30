#!/usr/bin/env python3
import re
import sys
import argparse
from logging import Logger, basicConfig, getLogger
from os import getenv, environ, pathsep
from pathlib import Path
from typing import List, Set, Optional


logger = getLogger(__name__)  # type: Logger


class Expander:
    include_guard = re.compile(r'#.*ATCODER_[A-Z_]*_HPP')

    def is_ignored_line(self, line) -> bool:
        if self.include_guard.match(line):
            return True
        if line.strip() == "#pragma once":
            return True
        if line.strip().startswith('//'):
            return True
        return False

    def __init__(self, lib_paths: List[Path], full: bool):
        self.lib_paths = lib_paths
        self.full = full
        self.include_pattern = re.compile(
            r'#include [<"](.*)[">]' if full else r'#include "(.*)"')

    included = set()  # type: Set[Path]

    def find_acl(self, acl_name: str) -> Path:
        for lib_path in self.lib_paths:
            path = lib_path / acl_name
            if path.exists():
                return path
        logger.warning('include not found, left as-is: {}'.format(acl_name))
        raise FileNotFoundError()

    def expand_acl(self, acl_file_path: Path) -> List[str]:
        if acl_file_path in self.included:
            logger.info('already included: {}'.format(acl_file_path.name))
            return []
        self.included.add(acl_file_path)
        logger.info('include: {}'.format(acl_file_path.name))

        acl_source = open(str(acl_file_path)).read()

        result = []  # type: List[str]
        for line in acl_source.splitlines():
            if self.is_ignored_line(line):
                continue

            m = self.include_pattern.match(line)
            if m:
                try:
                    acl_path = self.find_acl(m.group(1))
                    result.extend(self.expand_acl(acl_path))
                except FileNotFoundError:
                    result.append(line)
            else:
                result.append(line)
        return result

    def expand(self, source: str) -> str:
        self.included = set()
        result = []  # type: List[str]
        for line in source.splitlines():
            m = self.include_pattern.match(line)
            if m:
                try:
                    acl_path = self.find_acl(m.group(1))
                    result.extend(self.expand_acl(acl_path))
                except FileNotFoundError:
                    result.append(line)
            else:
                result.append(line)
        return '\n'.join(result)


if __name__ == "__main__":
    basicConfig(
        format="%(asctime)s [%(levelname)s] %(message)s",
        datefmt="%H:%M:%S",
        level=getenv('LOG_LEVEL', 'INFO'),
    )
    parser = argparse.ArgumentParser(description='Expander')
    parser.add_argument('source', help='Source File')
    parser.add_argument('--lib', '-I', action='append', help='Paths to include')
    parser.add_argument('--full', '-f', action='store_true', help='Expand All Header')
    opts = parser.parse_args()

    lib_paths = [Path(p) for p in opts.lib]
    lib_paths.append(Path(opts.source).parent)
    logger.info(lib_paths)
    expander = Expander(lib_paths, opts.full)
    source = open(opts.source).read()
    output = expander.expand(source)
    print(output)
