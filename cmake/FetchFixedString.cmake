include(FetchContent)

FetchContent_Declare(
    fixed_string
    GIT_REPOSITORY https://github.com/unterumarmung/fixed_string.git
    GIT_TAG v0.1.1)

FetchContent_MakeAvailable(fixed_string)
