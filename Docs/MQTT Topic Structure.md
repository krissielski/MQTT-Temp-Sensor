# MQTT Topic Structure Documentation

## Topic Format

```
root/site/area/sensor-type/deviceID/topic
```

## Hierarchy Levels

### 1. Root
- **Purpose**: Top-level namespace for all IoT devices
- **Fixed value**: iot

### 2. Site
- **Purpose**: Primary location/deployment site
- **Examples**: home, work

### 3. Area
- **Purpose**: Specific area or room within a site
- **Examples**: office, kitchen, garage

### 4. Sensor Type
- **Purpose**: Category of sensor or measurement type
- **Examples**: temp

### 5. Device ID
- **Purpose**: Unique identifier for individual device

### 6. Topic
- **Purpose**: Message content
- **Standard topics**:
  - info - Device info
  - data - Sensor data


## Topic Examples
```
iot/home/kitchen/temp/temp-001/info
iot/home/garage/temp/temp-003/data
iot/work/office/temp/temp-042/data
```
