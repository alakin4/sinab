use super::*;

#[derive(Debug, Default)]
pub(crate) struct LayoutDataForElement {
    pub(super) self_box: Option<LayoutBox>,
    pub(super) pseudo_elements: Option<Box<PseudoElementBoxes>>,
}

#[derive(Debug, Default)]
pub(super) struct PseudoElementBoxes {
    pub before: Option<LayoutBox>,
    pub after: Option<LayoutBox>,
}

#[derive(Debug)]
pub(super) enum LayoutBox {
    DisplayContents,
    BlockLevel(Arc<BlockLevelBox>),
    InlineLevel(Arc<InlineLevelBox>),
}
