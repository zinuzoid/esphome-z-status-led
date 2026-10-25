# esphome-z-status-led

ESPHome external components.

## `z_status_led`

A drop-in alternative to ESPHome's [`status_led`](https://esphome.io/components/status_led/):

| Device state | LED behaviour |
|---|---|
| Error | Fast blink (same as `status_led`) |
| Warning | Slow blink (same as `status_led`) |
| OK | Heartbeat: `count` short blinks every `interval` |

### Usage

```yaml
external_components:
  - source: github://zinuzoid/esphome-z-status-led@main
    components: [z_status_led]

z_status_led:
  pin: GPIO15
  heartbeat:
    interval: 5s
```

### Configuration

- **pin** (**Required**, [Pin Schema](https://esphome.io/guides/configuration-types/#pin-schema)): The LED pin. Supports `inverted: true`.
- **heartbeat** (*Optional*):
  - **interval** (*Optional*, Time): How often the heartbeat fires. Default `60s`.
  - **on_length** (*Optional*, Time): How long each blink is on. Default `100ms`.
  - **off_length** (*Optional*, Time): Gap after each blink. Default `100ms`.
  - **count** (*Optional*, int 0–10): Blinks per heartbeat. Default `2`. `0` keeps the LED off when healthy (like `status_led`).

Don't use `z_status_led` and `status_led` on the same pin.
