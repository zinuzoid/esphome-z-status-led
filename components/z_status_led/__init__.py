import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import CONF_ID, CONF_INTERVAL, CONF_INVERTED, CONF_NUMBER, CONF_PIN
from esphome.core import CoroPriority, coroutine_with_priority

CODEOWNERS = ["@zinuzoid"]

CONF_HEARTBEAT = "heartbeat"
CONF_ON_LENGTH = "on_length"
CONF_OFF_LENGTH = "off_length"
CONF_COUNT = "count"

z_status_led_ns = cg.esphome_ns.namespace("z_status_led")
ZStatusLED = z_status_led_ns.class_("ZStatusLED", cg.Component)


def _validate_heartbeat(config):
    pattern_ms = (
        config[CONF_ON_LENGTH].total_milliseconds
        + config[CONF_OFF_LENGTH].total_milliseconds
    ) * config[CONF_COUNT]
    if pattern_ms > config[CONF_INTERVAL].total_milliseconds:
        raise cv.Invalid(
            f"Heartbeat pattern ({pattern_ms}ms) is longer than the interval"
        )
    return config


HEARTBEAT_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.Optional(CONF_INTERVAL, default="2s"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_ON_LENGTH, default="100ms"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_OFF_LENGTH, default="100ms"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_COUNT, default=2): cv.int_range(min=0, max=10),
        }
    ),
    _validate_heartbeat,
)


def heartbeat_schema(value):
    # An empty `heartbeat:` uses the defaults.
    if value is None:
        value = {}
    return HEARTBEAT_SCHEMA(value)


def _merge_inverted(config):
    # Top-level `inverted:` is shorthand for the pin's `inverted:`.
    if not isinstance(config, dict) or CONF_INVERTED not in config:
        return config
    config = dict(config)
    inverted = cv.boolean(config.pop(CONF_INVERTED))
    pin = config.get(CONF_PIN)
    if pin is None:
        return config
    pin = dict(pin) if isinstance(pin, dict) else {CONF_NUMBER: pin}
    if CONF_INVERTED in pin and cv.boolean(pin[CONF_INVERTED]) != inverted:
        raise cv.Invalid("`inverted` conflicts with the pin's `inverted`")
    pin[CONF_INVERTED] = inverted
    config[CONF_PIN] = pin
    return config


CONFIG_SCHEMA = cv.All(
    _merge_inverted,
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ZStatusLED),
            cv.Required(CONF_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_HEARTBEAT, default={}): heartbeat_schema,
        }
    ).extend(cv.COMPONENT_SCHEMA),
)


@coroutine_with_priority(CoroPriority.STATUS)
async def to_code(config):
    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    var = cg.new_Pvariable(config[CONF_ID], pin)
    await cg.register_component(var, config)
    cg.add(var.pre_setup())

    heartbeat = config[CONF_HEARTBEAT]
    cg.add(var.set_heartbeat_interval(heartbeat[CONF_INTERVAL]))
    cg.add(var.set_heartbeat_on_length(heartbeat[CONF_ON_LENGTH]))
    cg.add(var.set_heartbeat_off_length(heartbeat[CONF_OFF_LENGTH]))
    cg.add(var.set_heartbeat_count(heartbeat[CONF_COUNT]))
