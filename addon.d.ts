export function readDictionary(path_to_dictionary: string): Promise<Dictionary>;
export function synthesis(text: string, options: OpenJTalkOptions): Promise<WaveObject>;
export { path_to_dictionary as dictionary_dir };
export type Dictionary = {
    /**
     * unk.dic
     */
    unkdic: Uint8Array | ArrayBuffer;
    /**
     * sys.dic
     */
    sysdic: Uint8Array | ArrayBuffer;
    /**
     * char.bin
     */
    property: Uint8Array | ArrayBuffer;
    /**
     * matrix.bin
     */
    matrix: Uint8Array | ArrayBuffer;
};
export type OpenJTalkOptions = {
    /**
     * Path to htsvoice. Or data ArrayBuffer,Buffer.
     */
    htsvoice: string | Uint8Array | ArrayBuffer;
    /**
     * Dictionary object or Path to dictionary. NOT be URL nor Buffer. Must be encoded by UTF-8. The default is to use dictionary_dir.
     */
    dictionary?: string | Dictionary;
    /**
     * Must be int. 1<=sampling_frequency.
     */
    sampling_frequency?: number;
    /**
     * Must be int. 1<=frame_period.
     */
    frame_period?: number;
    /**
     * 0.0<=all_pass_constant<=1.0.
     */
    all_pass_constant?: number;
    /**
     * Default is 0.0. 0.0<=postfiltering_coefficient<=1.0.
     */
    postfiltering_coefficient?: number;
    /**
     * Default is 1.0. 0<=speech_speed_rate. Warning: Do not set a very small value as it consumes CPU time.
     */
    speech_speed_rate?: number;
    /**
     * Default is 0.0.
     */
    additional_half_tone?: number;
    /**
     * Default is 0.5. 0.0<=voiced_unvoiced_threshold<=1.0.
     */
    voiced_unvoiced_threshold?: number;
    /**
     * Default is 1.0. 0.0<=weight_of_GV_for_spectrum.
     */
    weight_of_GV_for_spectrum?: number;
    /**
     * Default is 1.0. 0.0<=weight_of_GV_for_log_F0.
     */
    weight_of_GV_for_log_F0?: number;
    /**
     * Default is 0.0.
     */
    volume_in_dB?: number;
    /**
     * Disabled as default. 0 regarded disabled. 0<=audio_buffer_size
     */
    audio_buffer_size?: number;
};
export type WaveObject = {
    /**
     * Synthesized PCM by host byte order.
     */
    raw_data: Buffer;
    /**
     * Synthesized PCM.
     */
    data: Int16Array;
    /**
     * LINEAR16.
     */
    bitDepth: 16;
    /**
     * Equals to OpenJTalkOptions#sampling_frequency if presented for synthesis function. Else automatically determined value.
     */
    sampleRate: number;
    /**
     * monaural.
     */
    numChannels: 1;
};
declare const path_to_dictionary: string;
