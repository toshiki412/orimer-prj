#pragma once

namespace orimer::button {

/**
 * @brief ボタン入力初期化
 */
void Initialize();

/**
 * @brief ボタンが押されているか（raw）
 * @return true  押下中
 * @return false 離されている
 */
bool IsPressed();

} // namespace orimer::button
