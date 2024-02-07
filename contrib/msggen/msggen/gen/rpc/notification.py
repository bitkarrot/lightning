import logging
from textwrap import dedent, indent
from typing import Any, Dict, Optional, TextIO

from msggen.model import Service
from msggen.gen.generator import IGenerator
from msggen.gen.rpc.rust import gen_composite


class NotificationGenerator(IGenerator):

    def __init__(self, dest: TextIO, meta: Dict[str, Any]):
        self.dest = dest
        self.logger = logging.getLogger(__name__)
        self.meta = meta

    def write(self, text: str, numindent: Optional[int] = None) -> None:
        raw = dedent(text)
        if numindent is not None:
            raw = indent(text, "    " * numindent)

        self.dest.write(raw)

    def generate_enum(self, service: Service):
        self.write("#[derive(Clone, Debug, Deserialize, Serialize)]\n")
        self.write("pub enum Notification {\n")
        for notification in service.notifications:
            tn = notification.typename
            name = notification.name
            self.write(f'#[serde(rename = "{name}")]\n', numindent=1)
            self.write(f"{tn}({tn}Notification),\n", numindent=1),
        self.write("}\n")

    def generate(self, service: Service) -> None:
        self.write("// This file is autogenerated by `msggen`\n")
        self.write("// Do not edit it manually, your changes will be overwritten\n\n\n")
        self.write("\n")
        self.write("use crate::primitives::*;\n")
        self.write("use serde::{Serialize, Deserialize};\n")

        self.generate_enum(service)
        self.write("\n\n")

        for notification in service.notifications:
            _, resp_decl = gen_composite(notification.response, self.meta)
            self.write(resp_decl)
