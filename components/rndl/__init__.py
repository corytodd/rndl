import logging
import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.const import (
    CONF_ID,
)

_LOGGER = logging.getLogger(__name__)

CODEOWNERS = ["@corytodd"]
CONF_RNDL = "rndl"

rndl_ns = cg.esphome_ns.namespace("rndl_ns")
Rndl = rndl_ns.class_("Rndl", cg.Component)

MULTI_CONF = True

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_ID): cv.declare_id(Rndl),
        }
    ).extend(cv.COMPONENT_SCHEMA),
)
