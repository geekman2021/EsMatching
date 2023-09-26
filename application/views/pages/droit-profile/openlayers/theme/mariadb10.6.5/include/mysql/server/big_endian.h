 $indexByDescription = 0;

        foreach ($keys as $indexByKeyname => $value) {
            [
                $html,
                $horizontal_count,
                $indexByDescription,
            ] = $this->getHtmlForOneKey(
                $horizontal_count,
                $header,
                $keys,
                $indexByKeyname,
                $descriptions,
                $indexByDescription,
                $current_value
            );
            $output .= $html;
        }

        $output .= '</tbody>'
            . '</table>';

        return $output;
    }

    /**
     * Get the description (possibly truncated) and the title
     *
     * @param string $description the key name's description
     *
     * @return array the new description and title
     */
    private function getDescriptionAndTitle(string $description): array
    {
        if (mb_strlen($description) <= $this->limitChars) {
            $description = htmlspecialchars(
                $description
            );
            $descriptionTitle = '';
        } else {
            $descriptionTitle = htmlspecialchars(
                $description
            );
            $description = htmlspecialchars(
                mb_substr(
                    $description,
                    0,
                    $this->limitChars
                )
                . '...'
            );
        }

        return [
            $description,
            $descriptionTitle,
        ];
    }

    /**
     * Function to get html for the goto page option
     *
     * @param array|null $foreignData foreign data
     */
    private function getHtmlForGotoPage(?array $foreignData): string
    {
        $gotopage = '';
        isset($_POST['pos']) ? $pos = $_POST['pos'] : $pos = 0;
        if ($foreignData === null || ! is_array($foreignData['disp_row'])) {
            return $gotopage;
        }

        $pageNow = (int) floor($pos / $this->maxRows) + 1;
        $nbTotalPage = (int) ceil($foreignData['the_total'] / $this->maxRows);

        if ($foreignData['the_total'] > $this->maxRows) {
            $gotopage = Util::pageselector(
                'pos',
                $this->maxRows,
                $pageNow,
                $nbTotalPage,
                200,
                5,
                5,
                20,
                10,
                __('Page number:')
            );
        }

        return $gotopage;
    }

    /**
     * Function to get foreign limit
     *
     * @param string|null $foreignShowAll foreign navigation
     */
    public function getForeignLimit(?string $foreignShowAll): ?string
    {
        if (isset($foreignShowAll) && $foreignShowAll == __('Show all')) {
            return null;
        }
        isset($_POST['pos']) ? $pos = $_POST['pos'] : $pos = 0;

        return 'LIMIT ' . $pos . ', ' . $this->maxRows . ' ';
    }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     <?php
/**
 * Used to render the console of PMA's pages
 */

declare(strict_types=1);

namespace PhpMyAdmin;

use function count;
use function sprintf;

/**
 * Class used to output the console
 */
class Console
{
    /**
     * Whether to display anything
     *
     * @access private
     * @var bool
     */
    private $isEnabled;

    /**
     * Whether we are servicing an ajax request.
     *
     * @ac