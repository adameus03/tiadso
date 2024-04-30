project('zad3', 'c')

#add_project_arguments('-g', language: 'c')
#add_project_arguments('-O2', language: 'c')

#mdep = dependency('m', method : 'build-in') #https://stackoverflow.com/questions/60927711/how-to-set-meson-link-dependency-for-buildin-lib-as-lm-lrt - seems not to be working
cc = meson.get_compiler('c')
add_project_arguments('-mrdrnd', language: 'c')
add_project_arguments('-mavx512f', language: 'c')
mdep = cc.find_library('m', required : false)
executable('zad3', [
    'main.c', 
    'commons/log.c', 
    'commons/sol.c',
    'loss_functions.c'
], dependencies : [mdep]) #https://stackoverflow.com/questions/49545752/error-did-you-compile-with-rdynamic-seen-only-when-application-installed-usi