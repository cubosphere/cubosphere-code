# Before upload [done]
 * [x] Setup CMake build
 * [x] Separate data to another repo

# Soon after upload [in progress]
 * [x] Replace header guards with `#pragma once`
 * [x] Update credits/links
 * [x] Update license
 * [ ] Update various Readme's
 * [x] Improve modding support with CMake [bad idea, changed]
 * [x] Create icon
 * [x] Add .dekstop to menu
 * [x] Fix warnings (some are bugs-related) and C++17 errors…
 * [x] …and build with C++17 standard
 * [x] Setup CI build [Circle CI done, Travis CI cancelled]
 * [x] Separate bin and data packages (use data repo hashes for `-data` package)

# After base setup [partially done]
 * [ ] Use zlib instead of integrated copy (it will drop encryption, but it isn't important I think)
 * [x] Stop using std!
 * [ ] Add Russian transation
 * [x] Make .deb package… [done with CI]
 * [x] …and post it to PPA (or other storage, <http://cubosphere.vallua.ru/artifacts>, becasue PPA need source packages)
 * [ ] Make windows build (Circle CI with cross-build or AppVeyor)
 * [ ] Make OS X build
 * [ ] Invite people for testing

# Meanwhile
 * [ ] Use other music format? [mp3 isn't very good]
 * [ ] Contact original developers for some hints (How to translate some secrets? I didn't solved them)
 * [ ] Finally setup PPA (may be hard)
 * [ ] Add support for installing mods in user home
 * [ ] Migrate to SDL2 (seems not so hard)
 * [ ] Use modern C++ (17 or next one if ready)
 * [ ] Do some TODO's from code
 * [ ] Introduce unit testing
 * [ ] Migrate to Vulkan
 * [ ] Check/Add/Fix Lua 5.2/5.3(/5.4?) compatiblity

# In far future (or probably never in fact… but I belive)
 * [ ] Invite more developers
 * [ ] Invite designers to extend level set and improve exsisting levels
 * [ ] Make online user levels store…
 * [ ] …and integate it to game (for upload/download)
