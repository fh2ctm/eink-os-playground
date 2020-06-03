#include "PhonebookSearchResults.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/widgets/PhonebookItem.hpp"

#include <service-appmgr/ApplicationManager.hpp>

namespace gui
{
    PhonebookSearchResults::PhonebookSearchResults(app::Application *app)
        : AppWindow(app, gui::window::name::search_results), searchResultsModel{new SearchResultsModel(app)}
    {
        buildInterface();
    }

    void PhonebookSearchResults::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void PhonebookSearchResults::buildInterface()
    {

        AppWindow::buildInterface();

        searchResultList = new gui::PhonebookListView(this,
                                                      phonebookStyle::searchResults::searchResultList::x,
                                                      phonebookStyle::searchResults::searchResultList::y,
                                                      phonebookStyle::searchResults::searchResultList::w,
                                                      phonebookStyle::searchResults::searchResultList::h);
        searchResultList->setMaxElements(phonebookStyle::searchResults::searchResultList::maxElements);
        searchResultList->setPageSize(phonebookStyle::searchResults::searchResultList::pageSize);
        searchResultList->setPenFocusWidth(phonebookStyle::searchResults::searchResultList::penFocusWidth);
        searchResultList->setPenWidth(phonebookStyle::searchResults::searchResultList::penWidth);
        searchResultList->setProvider(searchResultsModel);
        searchResultList->setApplication(application);

        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_phonebook_call"));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_open"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

        topBar->setActive(TopBar::Elements::TIME, true);

        setTitle(utils::localize.get("common_results_prefix"));
    }

    void PhonebookSearchResults::destroyInterface()
    {
        erase();
        delete searchResultsModel;
    }

    PhonebookSearchResults::~PhonebookSearchResults()
    {
        destroyInterface();
    }

    void PhonebookSearchResults::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        setFocusItem(searchResultList);
        searchResultsModel->requestFavouritesCount();
    }

    auto PhonebookSearchResults::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        // process only if key is released
        if (inputEvent.state != InputEvent::State::keyReleasedShort) {
            return false;
        }
        if (inputEvent.state == InputEvent::State::keyReleasedShort) {
            switch (inputEvent.keyCode) {
            case KeyCode::KEY_LEFT:
                LOG_INFO("Adding new contact");
                application->switchWindow(gui::window::name::new_contact);
                return true;
            case KeyCode::KEY_RIGHT:
                LOG_INFO("Searching contact");
                application->switchWindow("Search");
                return true;
            default:
                break;
            }
        }

        return false;
    }

    auto PhonebookSearchResults::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto fillResults = [=](std::shared_ptr<std::vector<ContactRecord>> res, const std::string &title) {
            if (res == nullptr || res->size() == 0) {
                return;
            }
            searchResultsModel->setResults(res);
            searchResultList->clear();
            searchResultList->setElementsCount(res.get()->size());
            setTitle(utils::localize.get("common_results_prefix") + "\"" + title + "\"");
        };

        auto searchResults = dynamic_cast<PhonebookSearchResultsData *>(data);
        if (searchResults != nullptr) {
            fillResults(searchResults->getResults(), searchResults->getQuery());
            return true;
        }

        auto contactRequest = dynamic_cast<PhonebookSearchReuqest *>(data);
        if (contactRequest != nullptr) {
            searchResultList->cb_ENTER = [=](gui::PhonebookItem *item) {
                std::unique_ptr<PhonebookSearchReuqest> data = std::make_unique<PhonebookSearchReuqest>();
                data->result                                 = item->getContact();
                data->setDescription("PhonebookSearchRequest");
                return sapm::ApplicationManager::messageSwitchPreviousApplication(
                    application, std::make_unique<sapm::APMSwitchPrevApp>(application->GetName(), std::move(data)));
            };
            fillResults(contactRequest->results, contactRequest->request);
            setTitle(utils::localize.get("common_results_prefix") + "\"" + contactRequest->request + "\"");
        }

        return false;
    }
} /* namespace gui */
