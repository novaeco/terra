/*****************************************************************************
 * | File         :   sd.c
 * | Author       :   Waveshare team
 * | Function     :   SD card driver code for mounting, reading capacity, and unmounting
 * | Info         :
 * |                  This is the C file for SD card configuration and usage.
 * ----------------
 * | This version :   V1.0
 * | Date         :   2024-11-28
 * | Info         :   Basic version, includes functions to initialize,
 * |                  read memory capacity, and manage SD card mounting/unmounting.
 *
 ******************************************************************************/

#include "sd.h"  // Include header file for SD card functions
#include "esp_log.h"
#include <string.h>
#include <strings.h>

// Global variable for SD card structure
static sdmmc_card_t *card;

// Define the mount point for the SD card
const char mount_point[] = MOUNT_POINT;

/**
 * @brief Initialize the SD card and mount the filesystem.
 * 
 * This function configures the SDMMC peripheral, sets up the host and slot, 
 * and mounts the FAT filesystem from the SD card.
 * 
 * @retval ESP_OK if initialization and mounting succeed.
 * @retval ESP_FAIL if an error occurs during the process.
 */
esp_err_t sd_mmc_init() {
    esp_err_t ret;

    // Configuration for mounting the FAT filesystem
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = EXAMPLE_FORMAT_IF_MOUNT_FAILED, // Format if mount fails
        .max_files = 5,                  // Max number of open files
        .allocation_unit_size = 16 * 1024 // Allocation unit size
    };

    ESP_LOGI(SD_TAG, "Initializing SD card");

    // Use the SDMMC peripheral for SD card communication
    ESP_LOGI(SD_TAG, "Using SDMMC peripheral");

    // Host configuration with default settings for high-speed operation
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    // Slot configuration for SDMMC
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1;
    slot_config.clk = EXAMPLE_PIN_CLK;
    slot_config.cmd = EXAMPLE_PIN_CMD;
    slot_config.d0 = EXAMPLE_PIN_D0;
    // Enable internal pull-ups on the GPIOs
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(SD_TAG, "Mounting filesystem");

    // Mount the filesystem and initialize the SD card
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(SD_TAG, "Failed to mount filesystem. "
                             "Format the card if mount fails.");
        } else {
            ESP_LOGE(SD_TAG, "Failed to initialize the card (%s). "
                             "Check pull-up resistors on the card lines.", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }
    ESP_LOGI(SD_TAG, "Filesystem mounted");
    return ret;
}

/**
 * @brief Print detailed SD card information to the console.
 * 
 * Uses the built-in `sdmmc_card_print_info` function to log information 
 * about the SD card to the standard output.
 */
void sd_card_print_info() {
    sdmmc_card_print_info(stdout, card);
}

/**
 * @brief Unmount the SD card and release resources.
 * 
 * This function unmounts the FAT filesystem and ensures all resources 
 * associated with the SD card are released.
 * 
 * @retval ESP_OK if unmounting succeeds.
 * @retval ESP_FAIL if an error occurs.
 */
esp_err_t sd_mmc_unmount() {
    return esp_vfs_fat_sdcard_unmount(mount_point, card);
}

/**
 * @brief Get total and available memory capacity of the SD card.
 * 
 * @param total_capacity Pointer to store the total capacity (in KB).
 * @param available_capacity Pointer to store the available capacity (in KB).
 * 
 * @retval ESP_OK if memory information is successfully retrieved.
 * @retval ESP_FAIL if an error occurs while fetching capacity information.
 */
esp_err_t read_sd_capacity(size_t *total_capacity, size_t *available_capacity) {
    FATFS *fs;
    uint32_t free_clusters;

    // Get the number of free clusters in the filesystem
    int res = f_getfree(mount_point, &free_clusters, &fs);
    if (res != FR_OK) {
        ESP_LOGE(SD_TAG, "Failed to get number of free clusters (%d)", res);
        return ESP_FAIL;
    }

    // Calculate total and free sectors based on cluster size
    uint64_t total_sectors = ((uint64_t)(fs->n_fatent - 2)) * fs->csize;
    uint64_t free_sectors = ((uint64_t)free_clusters) * fs->csize;

    // Convert sectors to size in KB
    size_t sd_total_KB = (total_sectors * fs->ssize) / 1024;
    size_t sd_available_KB = (free_sectors * fs->ssize) / 1024;

    // Store total capacity if the pointer is valid
    if (total_capacity != NULL) {
        *total_capacity = sd_total_KB;
    }

    // Store available capacity if the pointer is valid
    if (available_capacity != NULL) {
        *available_capacity = sd_available_KB;
    }

    return ESP_OK;
}

esp_err_t read_files(const char *base_path, int depth) 
{
    DIR *dir = opendir(base_path);
    if (!dir) {
        ESP_LOGE(SD_TAG, "Impossible d'ouvrir le répertoire: %s", base_path);
        return ESP_OK;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ignore . et ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construit le chemin complet
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);

        // Affiche avec indentation selon la profondeur
        for (int i = 0; i < depth; i++) {
            printf("  ");  // ou tu peux utiliser ESP_LOGI
        }
        ESP_LOGI(SD_TAG, "%s", entry->d_name);

        // Vérifie si c’est un dossier
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            // Si c’est un dossier → appel récursif
            read_files(path, depth + 1);
        }
    }
    closedir(dir);
    return ESP_OK;
}

// Retourne le nombre de fichiers trouvés, et remplit *out_files
int list_png_files(const char *base_path, char ***out_files) {
    DIR *dir = opendir(base_path);
    if (!dir) {
        printf("Erreur: impossible d'ouvrir le dossier %s\n", base_path);
        *out_files = NULL;
        return -1;
    }

    struct dirent *entry;
    int count = 0;

    // Allocation tableau de pointeurs
    char **files = calloc(MAX_FILES, sizeof(char *));
    if (!files) {
        closedir(dir);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL && count < MAX_FILES) {
        // On ignore les répertoires
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
            // Vérifier si le nom se termine par ".png" (insensible à la casse possible)
            const char *ext = strrchr(entry->d_name, '.');
            if (ext && (strcasecmp(ext, ".png") == 0)) {
                files[count] = strdup(full_path);
                if (!files[count]) {
                    // si erreur d'allocation on arrête
                    break;
                }
                count++;
            }
            else if (ext && (strcasecmp(ext, ".jpg") == 0)) {
                files[count] = strdup(full_path);
                if (!files[count]) {
                    // si erreur d'allocation on arrête
                    break;
                }
                count++;
            }
            else if (ext && (strcasecmp(ext, ".bmp") == 0)) {
                files[count] = strdup(full_path);
                if (!files[count]) {
                    // si erreur d'allocation on arrête
                    break;
                }
                count++;
            }
            else if (ext && (strcasecmp(ext, ".jpeg") == 0)) {
                files[count] = strdup(full_path);
                if (!files[count]) {
                    // si erreur d'allocation on arrête
                    break;
                }
                count++;
            }
        }
    }

    closedir(dir);

    *out_files = files;
    return count;
}
