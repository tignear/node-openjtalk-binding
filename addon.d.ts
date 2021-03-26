export function synthesis(text: string, options: OpenJTalkOptions): Promise<WaveObject>;
export var dictionary_dir: string;
export type OpenJTalkOptions = {
    /**
     * Path to dictionary. NOT be URL nor Buffer.
     */
    dictionary: string;
    /**
     * Path to htsvoice. NOT be URL nor Buffer.
     */
    htsvoice: string;
    /**
     * Must be int. 1<=sampling_frequency.
     */
    sampling_frequency?: number | null;
    /**
     * Must be int. 1<=frame_period.
     */
    frame_period?: number | null;
    /**
     * 0.0<=all_pass_constant<=1.0.
     */
    all_pass_constant?: number | null;
    /**
     * Default is 0.0. 0.0<=postfiltering_coefficient<=1.0.
     */
    postfiltering_coefficient?: number | null;
    /**
     * Default is 1.0. 0<=speech_speed_rate. Warning: Do not set a very small value as it consumes CPU time.
     */
    speech_speed_rate?: number | null;
    /**
     * Default is 0.0.
     */
    additional_half_tone?: number | null;
    /**
     * Default is 0.5. 0.0<=voiced_unvoiced_threshold<=1.0.
     */
    voiced_unvoiced_threshold?: number | null;
    /**
     * Default is 1.0. 0.0<=weight_of_GV_for_spectrum.
     */
    weight_of_GV_for_spectrum?: number | null;
    /**
     * Default is 1.0. 0.0<=weight_of_GV_for_log_F0.
     */
    weight_of_GV_for_log_F0?: number | null;
    /**
     * Default is 0.0.
     */
    volume_in_dB?: number | null;
    /**
     * Disabled as default. 0 regarded disabled. 0<=audio_buffer_size
     */
    audio_buffer_size?: number | null;
};
export type WaveObject = {
    /**
     * Synthesized PCM by host byte order.
     */
    raw_data: any;
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
