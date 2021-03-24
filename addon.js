const { synthesis: _synthesis } = require("bindings")("addon");
const path = require("path");

/**
 * @typedef {Object} OpenJTalkOptions
 * @property {string} dictionary Path to dictionary. NOT be URL nor Buffer.
 * @property {string} htsvoice Path to htsvoice. NOT be URL nor Buffer.
 * @property {number|undefined} sampling_frequency Must be int. 1<=sampling_frequency.
 * @property {number|undefined} frame_period Must be int. 1<=frame_period.
 * @property {number|undefined} all_pass_constant 0.0<=all_pass_constant<=1.0.
 * @property {number|undefined} postfiltering_coefficient Default is 0.0. 0.0<=postfiltering_coefficient<=1.0.
 * @property {number|undefined} speech_speed_rate Default is 1.0. 0<=speech_speed_rate. Warning: Do not set a very small value as it consumes CPU time.
 * @property {number|undefined} additional_half_tone Default is 0.0.
 * @property {number|undefined} voiced_unvoiced_threshold Default is 0.5. 0.0<=voiced_unvoiced_threshold<=1.0.
 * @property {number|undefined} weight_of_GV_for_spectrum Default is 1.0. 0.0<=weight_of_GV_for_spectrum.
 * @property {number|undefined} weight_of_GV_for_log_F0 Default is 1.0. 0.0<=weight_of_GV_for_log_F0.
 * @property {number|undefined} volume_in_dB Default is 0.0.
 * @property {number|undefined} audio_buffer_size Disabled as default. 0 regarded disabled. 0<=audio_buffer_size
 */


/**
 * Synthesis voice with OpenJTalk
 * @param {string} text Text to synthesize.
 * @param {OpenJTalkOptions} options OpenJTalk synthesize option.
 * @return {Promise<Buffer>} Synthesized PCM by host byte order.
 */
function synthesis(text, options) {
  return new Promise((resolve, reject) => {
    try {
      _synthesis((err, buffer) => {
        if (err) {
          reject(err);
          return;
        }
        resolve(buffer);
      }, text, options);
    } catch (err) {
      reject(err);
    }
  });
}

/** @const {string} dictionary_dir Path to builded dictionary.*/
const dictionary_dir = path.resolve(__dirname, "openjtalk", "mecab-naist-jdic");

/* exports */
exports.synthesis = synthesis;
exports.dictionary_dir = dictionary_dir;