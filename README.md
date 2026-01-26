# Solar System

Gravitational physics simulation with procedurally generated planets using icosphere generation and Perlin noise terrain.

## Overview

- **Realistic orbital mechanics** using gravitation
- **Orbit path visualization** with customizable colors per planet
- **Procedural planet generation** using icosphere subdivision
- **Perlin noise terrain** with customizable height and detail

### Physics Features
- Gravitational force calculations
- Orbit prediction and visualization
- Real-time position and velocity updates
- Time scaling for faster/slower simulation

### Procedural Generation
- Icosahedron-based sphere generation (20 base triangles)
- Recursive subdivision for detail levels

## Configuration Parameters

**Celestial Body Settings:**
- `BodyName`: Planet identifier
- `Mass`: Body mass (proportional values)
- `Radius`: Physical radius for gravity calculations
- `InitialVelocity`: Starting velocity vector
- `OrbitColor`: Color for orbit path visualization

**Solar System Manager:**
- `TimeScale`: Simulation speed multiplier
- `drawOrbits`: Enable/disable orbit path visualization
- `detailedLogs`: Enable more detailed logging

**Procedural Planet Settings:**
- `UseProcedural`: Toggle between static mesh and procedural generation
- `Radius`: Planet size

**Terrain Noise Settings:**
- `ApplyNoise`: Enable/disable terrain generation
- `NoiseScale`: Frequency/zoom
- `NoiseHeightMultiplier`: Height variation
- `NoiseOctaves`: Detail layers
- `NoisePersistence`: Roughness control
- `NoiseLacunarity`: Frequency multiplier between octaves
- `NoiseSeed`: Random seed for unique terrain patterns