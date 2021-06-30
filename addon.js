/**
 * Simple binding of OpenJTalk.
 * @module node-openjtalk-binding
 */
const binary = require("@tignear/node-pre-gyp");
const path = require('path');
const meta = binary.meta(path.resolve(path.join(__dirname, './package.json')));
const { synthesis: _synthesis } = require(meta.module);
const { promises: fs } = require("fs");
const path_to_dictionary = path.resolve(path.join(meta.module_path, 'dictionary'));

/**
 * @typedef {Object} Dictionary
 * @property {Uint8Array|ArrayBuffer} unkdic unk.dic
 * @property {Uint8Array|ArrayBuffer} sysdic sys.dic
 * @property {Uint8Array|ArrayBuffer} property char.bin
 * @property {Uint8Array|ArrayBuffer} matrix matrix.bin
 */

/**
 * @typedef {Object} OpenJTalkOptions
 * @property {!string|Uint8Array|ArrayBuffer} htsvoice Path to htsvoice. Or data ArrayBuffer,Buffer.
 * @property {string|Dictionary} [dictionary] Dictionary object or Path to dictionary. NOT be URL nor Buffer. Must be encoded by UTF-8. The default is to use dictionary_dir.
 * @property {number} [sampling_frequency] Must be int. 1<=sampling_frequency.
 * @property {number} [frame_period] Must be int. 1<=frame_period.
 * @property {number} [all_pass_constant] 0.0<=all_pass_constant<=1.0.
 * @property {number} [postfiltering_coefficient] Default is 0.0. 0.0<=postfiltering_coefficient<=1.0.
 * @property {number} [speech_speed_rate] Default is 1.0. 0<=speech_speed_rate. Warning: Do not set a very small value as it consumes CPU time.
 * @property {number} [additional_half_tone] Default is 0.0.
 * @property {number} [voiced_unvoiced_threshold] Default is 0.5. 0.0<=voiced_unvoiced_threshold<=1.0.
 * @property {number} [weight_of_GV_for_spectrum] Default is 1.0. 0.0<=weight_of_GV_for_spectrum.
 * @property {number} [weight_of_GV_for_log_F0] Default is 1.0. 0.0<=weight_of_GV_for_log_F0.
 * @property {number} [volume_in_dB] Default is 0.0.
 * @property {number} [audio_buffer_size] Disabled as default. 0 regarded disabled. 0<=audio_buffer_size
 */

/**
 * @typedef {Object} WaveObject
 * @property {!Buffer} raw_data  Synthesized PCM by host byte order. 
 * @property {!Int16Array} data Synthesized PCM.
 * @property {!16} bitDepth LINEAR16.
 * @property {!number} sampleRate Equals to OpenJTalkOptions#sampling_frequency if presented for synthesis function. Else automatically determined value.
 * @property {!1} numChannels monaural.
 */

/**
 * Read mecab dictionary.
 * @async
 * @function
 * @static
 * @param {String} path_to_dictionary 
 * @returns {Promise<Dictionary>}
 */
const readDictionary = exports.readDictionary = async function readDictionary(path_to_dictionary) {
  const [unkdic, sysdic, property, matrix] = (await Promise.all(
    [
      fs.readFile(path.resolve(path_to_dictionary, "unk.dic")),
      fs.readFile(path.resolve(path_to_dictionary, "sys.dic")),
      fs.readFile(path.resolve(path_to_dictionary, "char.bin")),
      fs.readFile(path.resolve(path_to_dictionary, "matrix.bin"))
    ]
  )).map(e => e.buffer);
  return {
    unkdic,
    sysdic,
    property,
    matrix
  };
}

const default_dictionary = readDictionary(path_to_dictionary);

/** 
 * Synthesis voice with OpenJTalk
 * @async
 * @function
 * @static
 * @param {string} text Text to synthesize.
 * @param {OpenJTalkOptions} options OpenJTalk synthesize option.
 * @return {Promise<WaveObject>} Synthesized PCM.
 */
exports.synthesis = async function synthesis(text, options) {
  let htsvoice = options.htsvoice;
  if (typeof htsvoice === "string") {
    htsvoice = await fs.readFile(htsvoice);
  }
  if (htsvoice instanceof Uint8Array) {
    htsvoice = htsvoice.buffer;
  }
  let dictionary = options.dictionary;
  if (!dictionary) {
    dictionary = await default_dictionary;
  } else if (typeof dictionary == "string") {
    dictionary = await readDictionary(dictionary);
  } else {
    dictionary = Object.fromEntries(Object.entries(dictionary).map(([k, v]) => [k, v instanceof Uint8Array ? v.buffer : v]));
  }
  return new Promise((resolve, reject) => {
    if (!text) reject(new TypeError("The first argument must be a non-empty string"));
    function cb(err, /** @type {Buffer} */ buffer, /** @type {number} */ sampleRate) {
      if (err) {
        reject(err);
        return;
      }
      /**
       * @type {WaveObject}
       */
      const wave = {
        raw_data: buffer,
        data: new Int16Array(buffer.buffer),
        bitDepth: 16,
        numChannels: 1,
        sampleRate
      };
      resolve(wave);
    }
    try {
      _synthesis(cb, text, { ...options, htsvoice, dictionary });
    } catch (err) {
      reject(err);
    }
  });
}


/** 
 * Path to builded dictionary.
 * @type {string} 
 * */
exports.dictionary_dir = path_to_dictionary;
