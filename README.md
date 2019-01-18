**Rockit** is a lightweight multimedia plugin system with the following features:

- Has a unified operating system adaptation layer.
- Streamlined plugin layer to minimize external dependencies; plugin layer has separate interfaces, plugins can be used alone or in tandem.
- Full-featured demultiplexing plugin, soft plugin and hard plugin.
- Support for extensions such as OpenCV plugins, Tensorflow checksums, etc.

## Architecture

![Architecture of Rockit](doc/images/rockit-arch.png)

## Project Files

- include: header file
- src/rt_base: base component
- src/rt_node: plugin management framework and plugin implementation
- src/rt_player: external interface
- src/rt_task: task thread and event bus
- doc: documentation
- tools: version management tool
- build/CodeBlock: CodeBlock project configuration
- build/Android: CodeBlock project configuration

## Plugin List

| Process | Plugin Name | Authors | Quality Rank\(0-5\) |  
| :---: | :--- | :---: | :---: |  
| 80% | Node API | @martin & @rimon.xu | ☆☆☆ |  
| 90% | Meta Extractor | @martin | ☆☆☆ |
| 80% | Demux Plugin  | @hery.xu & @martin  | ☆☆☆ |  
| 10% | Muxer Plugin | @hery.xu | ☆ |  
| 80% | Soft Codec  | @rimon.xu  | ☆☆☆ |  
| TODO | Hard Codec  | @rimon.xu  | 0 |
| 30% | Audio Sink  | @shika.zhou  | ☆☆☆ |  
| 30% | Video Sink  | @martin  | ☆☆☆ |
| TODO | Filter Plugin  | Unkown  | 0 |
| 30% | NodeBus  | @princejay.dai & @martin  | ☆☆☆ |
| TODO | IPTV Player  | Unkown  | 0 |
| TODO | Android Player  | Unkown  | 0 |

## License

[Apache License 2.0](LICENSE)
